/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "cmd.h"
#include "screen.h"
#include "parse.h"
#include "stack.h"

// DEFINES

#define TAB_STD     0
#define TAB_EXTRA   1
#define TAB_FANCY   2

// FORWARDS

struct cmd_ret std_escape_init(int given, int param);
struct cmd_ret std_insert_init(int given, int param);
struct cmd_ret std_test_init(int given, int param);
struct cmd_ret std_sub_init(int given, int param);
struct cmd_ret std_push_init(int given, int param);
struct cmd_ret std_extra_init(int given, int param);

struct cmd_ret extra_exit_init(int given, int param);

// VARIABLES

cmd_init* cmd_table[3][256];
int table_current;

// EXTERNAL

void cmd_start()
{
    table_current = TAB_STD;
    
    cmd_table[TAB_STD][27]  = std_escape_init;
    cmd_table[TAB_STD]['e'] = std_extra_init;
    cmd_table[TAB_STD]['i'] = std_insert_init;
    cmd_table[TAB_STD]['t'] = std_test_init;
    cmd_table[TAB_STD]['-'] = std_sub_init;
    cmd_table[TAB_STD][','] = std_push_init;
    
    cmd_table[TAB_EXTRA]['x'] = extra_exit_init;
}

cmd_init* cmd_lookup(char c)
{
    return cmd_table[table_current][c];
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

void empty_rubout()
{
    // do nothing
}

int empty(char* str)
{
    stack_push_p(empty_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
}

// == STD CMDs ==

// ESCAPE
// init:    execute top of func stack, if empty start prompt cleanup

void std_escape_rubout()
{
    cmd_rubout* r;
    stack_pop(&r, parse_get_stk_rub());
    cmd_exec* f;
    stack_pop(&f, parse_get_stk_rub());
    stack_push_s(&f, parse_get_stk_func());
    r();
}

struct cmd_ret std_escape_init(int given, int param)
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
        if(r.ret&CMD_MASK_RET == CMD_RET_FAILURE)
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
// init:    if param given insert ascii char of param at dot, else register ecf
// ecf:     insert new character at dot
// func[0]: do nothing

void std_insert_ecf(char c)
{
    screen_input_text(c);
}

void std_insert_ecf_rubout(char* text)
{
    screen_delete_text();
}

void std_insert_rubout()
{
    char mode;
    stack_pop(&mode, parse_get_stk_rub());
    if(mode == '0')
    {
        screen_delete_text();
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

struct cmd_ret std_insert_init(int given, int param)
{
    char mode;
    if(given)
    {
        if(param < 0 || param > 255) return ret(CMD_RET_FAILURE, 0);
        screen_input_text((char)param);
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
// init: toggle sign of param for next cmd

void std_sub_rubout()
{
    char toggle;
    stack_pop(&toggle, parse_get_stk_rub());
    if(toggle == '1') parse_toggle_sign();
}

struct cmd_ret std_sub_init(int given, int param)
{
    char toggle = param>0 ? '1' : '0';
    if(param>0) parse_toggle_sign();
    stack_push(&toggle, sizeof(char), parse_get_stk_rub());
    stack_push_p(std_sub_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS|CMD_MASK_RET, -1);
}

// PUSH
// init:    push param to gen stack

void std_push_rubout()
{
    stack_pop_s(parse_get_stk_gen());
}

struct cmd_ret std_push_init(int given, int param)
{
    stack_push(&param, sizeof(int), parse_get_stk_gen());
    stack_push_p(std_push_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS, 0);
}

// SCROLL
// init:    todo

void std_test_rubout()
{
    int n;
    stack_pop(&n, parse_get_stk_rub());
    screen_scroll(-n);
}

struct cmd_ret std_test_init(int given, int param)
{
    screen_scroll(param);
    stack_push(&param, sizeof(int), parse_get_stk_rub());
    stack_push_p(std_test_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    return ret(CMD_RET_SUCCESS, 0);
}

// EXTRA
// init: switch cmd table to EXTRA

void std_extra_rubout()
{
    stack_pop(&table_current, parse_get_stk_rub());
}

struct cmd_ret std_extra_init(int given, int param)
{
    stack_push(&table_current, sizeof(int), parse_get_stk_rub());
    stack_push_p(std_extra_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    
    table_current = TAB_EXTRA;
    
    return ret(CMD_RET_SUCCESS, 0);
}

// == EXTRA CMDs ==

// EXIT
// init:        put exit func on after stack
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

struct cmd_ret extra_exit_init(int given, int param)
{
    stack_push_sp(extra_exit_after, parse_get_stk_after());
    stack_push(&table_current, sizeof(int), parse_get_stk_rub());
    stack_push_p(extra_exit_rubout, sizeof(cmd_rubout*), parse_get_stk_rub());
    
    table_current = TAB_STD;
    
    return ret(CMD_RET_SUCCESS, 0);
}


