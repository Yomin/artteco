/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "cmd.h"
#include "parse.h"
#include "stack.h"
#include "buffer_mgr.h"
#include "screen.h"

#include <string.h>
#include <libgen.h>

// DEFINES

#define TAB_STD     0
#define TAB_EXTRA   1
#define TAB_FANCY   2

// FORWARDS

struct cmd_ret std_escape_main(int given, int param);
struct cmd_ret std_insert_main(int given, int param);
struct cmd_ret std_test_main(int given, int param);
struct cmd_ret std_sub_main(int given, int param);
struct cmd_ret std_push_main(int given, int param);
struct cmd_ret std_extra_main(int given, int param);

struct cmd_ret extra_exit_main(int given, int param);
struct cmd_ret extra_buffer_load_main(int given, int param);

// VARIABLES

cmd_main* cmd_table[3][256];
int table_current;

// EXTERNAL

void cmd_init()
{
    table_current = TAB_STD;
    
    cmd_table[TAB_STD][27]  = std_escape_main;
    cmd_table[TAB_STD]['e'] = std_extra_main;
    cmd_table[TAB_STD]['i'] = std_insert_main;
    cmd_table[TAB_STD]['t'] = std_test_main;
    cmd_table[TAB_STD]['-'] = std_sub_main;
    cmd_table[TAB_STD][','] = std_push_main;
    
    cmd_table[TAB_EXTRA]['x'] = extra_exit_main;
    cmd_table[TAB_EXTRA]['b'] = extra_buffer_load_main;
}

void cmd_finish()
{
    // nothing to do
}

cmd_main* cmd_lookup(char c)
{
    return cmd_table[table_current][(int)c];
}

void cmd_reset_table()
{
    table_current = TAB_STD;
}

// INTERNAL

struct cmd_ret ret(ret, value)
{
    struct cmd_ret s;
    s.ret = ret;
    s.value = value;
    return s;
}

struct cmd_ret empty(char* str);

void empty_rubout()
{
    stack_push_sp(empty, parse_get_stk_func());
}

struct cmd_ret empty(char* str)
{
    stack_push_p(empty_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS, 0);
}

// == STD CMDs ==

// ESCAPE
// main:    execute top of func stack, if empty start prompt cleanup

void std_escape_rubout()
{
    cmd_rubout* r;
    stack_pop(&r, parse_get_stk_rub());
    cmd_exec* f;
    stack_pop(&f, parse_get_stk_rub());
    stack_push_s(&f, parse_get_stk_func());
    r();
}

struct cmd_ret std_escape_main(int given, int param)
{
    if(stack_empty(parse_get_stk_func()))
    {
        return ret(CMD_RET_FINISH, 0);
    }
    else
    {
        cmd_exec* f;
        stack_pop(&f, parse_get_stk_func());
        stack_push(&f, sizeof(cmd_exec*), parse_get_stk_rub());
        struct cmd_ret r = f(stack_get_base(parse_get_stk_input()));
        if((r.ret & CMD_MASK_RET) == CMD_RET_FAILURE)
        {
            stack_pop_s(parse_get_stk_rub());
            stack_push(&f, sizeof(cmd_exec*), parse_get_stk_func());
        }
        else
        {
            stack_push_p(std_escape_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
        }
        return r;
    }
}

// INSERT
// main:    if param given insert ascii char of param at dot, else register ecf
// ecf:     insert new character at dot
// func[0]: do nothing

void std_insert_ecf(char c)
{
    char e = c < 32 ? '1' : '0';
    stack_push(&e, sizeof(char), parse_get_stk_rub());
    buffer_write_char(c, buffer_mgr_current());
}

void std_insert_ecf_rubout(char* text)
{
    char e;
    stack_pop(&e, parse_get_stk_rub());
    if(e == '1') buffer_delete_char(buffer_mgr_current());
    buffer_delete_char(buffer_mgr_current());
}

void std_insert_rubout()
{
    char mode;
    stack_pop(&mode, parse_get_stk_rub());
    if(mode == '0')
    {
        buffer_delete_char(buffer_mgr_current());
    }
    else
    {
        stack_pop_s(parse_get_stk_func());
        cmd_ecf* e;
        cmd_rubout* r;
        stack_pop(&e, parse_get_stk_rub());
        stack_pop(&r, parse_get_stk_rub());
        parse_set_ecf(e);
        parse_set_ecf_rub(r);
    }
}

struct cmd_ret std_insert_main(int given, int param)
{
    char mode;
    if(given)
    {
        if(param < 0 || param > 255) return ret(CMD_RET_FAILURE, 0);
        buffer_write_char((char)param, buffer_mgr_current());
        mode = '0';
    }
    else
    {
        stack_push_sp(empty, parse_get_stk_func());
        stack_push_p(parse_get_ecf(), sizeof(cmd_ecf*), parse_get_stk_rub());
        stack_push_p(parse_get_ecf_rub(), sizeof(cmd_ecf*), parse_get_stk_rub());
        parse_set_ecf(std_insert_ecf);
        parse_set_ecf_rub(std_insert_ecf_rubout);
        mode = '1';
    }
    stack_push(&mode, sizeof(char), parse_get_stk_rub());
    stack_push_p(std_insert_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    
    return ret(CMD_RET_SUCCESS, 0);
}

// SUB
// main: toggle sign of param for next cmd

void std_sub_rubout()
{
    char toggle;
    stack_pop(&toggle, parse_get_stk_rub());
    if(toggle == '1') parse_toggle_sign();
}

struct cmd_ret std_sub_main(int given, int param)
{
    char toggle = param>0 ? '1' : '0';
    if(param>0) parse_toggle_sign();
    stack_push(&toggle, sizeof(char), parse_get_stk_rub());
    stack_push_p(std_sub_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS|CMD_MASK_VALUE, -1);
}

// PUSH
// main:    push param to gen stack

void std_push_rubout()
{
    stack_pop_s(parse_get_stk_gen());
}

struct cmd_ret std_push_main(int given, int param)
{
    stack_push(&param, sizeof(int), parse_get_stk_gen());
    stack_push_p(std_push_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS, 0);
}

// SCROLL
// main:    todo

void std_test_rubout()
{
    int n;
    stack_pop(&n, parse_get_stk_rub());
    buffer_scroll(-n, buffer_mgr_current());
}

struct cmd_ret std_test_main(int given, int param)
{
    buffer_scroll(param, buffer_mgr_current());
    stack_push(&param, sizeof(int), parse_get_stk_rub());
    stack_push_p(std_test_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS, 0);
}

// EXTRA
// main: switch cmd table to EXTRA

void std_extra_rubout()
{
    stack_pop(&table_current, parse_get_stk_rub());
}

struct cmd_ret std_extra_main(int given, int param)
{
    stack_push(&table_current, sizeof(int), parse_get_stk_rub());
    stack_push_p(std_extra_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    
    table_current = TAB_EXTRA;
    
    if(given)
    {
        return ret(CMD_RET_SUCCESS|CMD_MASK_VALUE, param);
    }
    else
    {
        return ret(CMD_RET_SUCCESS, 0);
    }
}

// == EXTRA CMDs ==

// EXIT
// main:        put exit func on after stack
// after[0]:    exit teco

int extra_exit_after()
{
    return CMD_RET_EXIT;
}

void extra_exit_rubout()
{
    stack_pop_s(parse_get_stk_after());
    stack_pop(&table_current, parse_get_stk_rub());
}

struct cmd_ret extra_exit_main(int given, int param)
{
    stack_push_sp(extra_exit_after, parse_get_stk_after());
    stack_push(&table_current, sizeof(int), parse_get_stk_rub());
    stack_push_p(extra_exit_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    
    table_current = TAB_STD;
    
    return ret(CMD_RET_SUCCESS, 0);
}

// BUFFER LOAD
// main:    if param given switch buf or show stat else register buffer load function
// func[0]: load file in new buffer

void extra_buffer_load_rubout()
{
    stack_pop(&table_current, parse_get_stk_rub());
    char mode;
    stack_pop(&mode, parse_get_stk_rub());
    int previous;
    switch(mode)
    {
        case '0': break;
        case '1':
            stack_pop(&previous, parse_get_stk_rub());
            buffer_mgr_switch(previous);
            break;
        case '2':
            stack_pop_s(parse_get_stk_func());
            break;
    }
}

struct cmd_ret extra_buffer_load_func(char* str);

void extra_buffer_load_func_rubout()
{
    int previous, current = buffer_mgr_current()->number;
    stack_pop(&previous, parse_get_stk_rub());
    buffer_mgr_switch(previous);
    buffer_mgr_delete(current);
    stack_push_sp(extra_buffer_load_func, parse_get_stk_func());
}

struct cmd_ret extra_buffer_load_func(char* str)
{
    if(strlen(str) == 0)
    {
        screen_set_msg("filename required");
        return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
    }
    else
    {
        int current = buffer_mgr_current()->number;
        if(!buffer_mgr_add_file(basename(str), str))
        {
            screen_set_msg("file not found");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        }
        else
        {
            stack_push(&current, sizeof(int), parse_get_stk_rub());
            stack_push_p(extra_buffer_load_func_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
            return ret(CMD_RET_SUCCESS, 0);
        }
    }
}

struct cmd_ret extra_buffer_load_main(int given, int param)
{
    char mode;
    if(given)
    {
        if(param == 0)
        {
            mode = '0';
            screen_set_msg("not implemented");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        }
        else
        {
            mode = '1';
            int current = buffer_mgr_current()->number;
            if(buffer_mgr_switch(param))
            {
                stack_push(&current, sizeof(int), parse_get_stk_rub());
            }
            else
            {
                screen_set_msg("buffer not loaded");
                return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            }
        }
    }
    else
    {
        mode = '2';
        stack_push_sp(extra_buffer_load_func, parse_get_stk_func());
    }
    
    stack_push(&mode, sizeof(char), parse_get_stk_rub());
    stack_push(&table_current, sizeof(int), parse_get_stk_rub());
    stack_push_p(extra_buffer_load_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    table_current = TAB_STD;
    
    return ret(CMD_RET_SUCCESS, 0);
}
