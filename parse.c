/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "parse.h"
#include "screen.h"
#include "help.h"
#include "list.h"

#include <stdlib.h>
#include <ncurses.h>

// DEFINES

#define ESCAPE      27
#define BACKSPACE   127
#define ESC_CHAR    '$'

#define MASK_SIGN     1
#define MASK_ARG_CMD  2
#define MASK_ARG_USER 4

// FORWARDS

void ecf_empty(char c) {}
void ecf_rub_empty() {}

void process_param_num(char c);
void process_param_str(char c);
void process_rubout();
void process_cmd(char c);

// VARIABLES

struct stack_state stk_input, stk_gen, stk_func, stk_rub, stk_after;
cmd_ecf* ecf;
cmd_rubout* ecf_rub;
int status;                 // if >= 0 exit
int sign;                   // sign for next argument
int arg_given, arg_value;   // argument from last for next cmd

// EXTERNAL

void parse_init()
{
    stack_init(STACK_MODE_SIMPLE, sizeof(char), &stk_input);
    stack_init(STACK_MODE_SIMPLE, sizeof(cmd_exec*), &stk_func);
    stack_init(STACK_MODE_SIMPLE, sizeof(cmd_after*), &stk_after);
    
    stack_init(STACK_MODE_EXT, sizeof(int), &stk_gen);
    stack_init(STACK_MODE_EXT, sizeof(cmd_rubout*), &stk_rub);
    
    ecf = ecf_empty;
    ecf_rub = ecf_rub_empty;
    
    status = -1;
    sign = 1;
    arg_given = 0;
}

void parse_finish()
{
    // nothing to do
}

int parse_start()
{
    while(status < 0)
    {
        char c = getch();
        if(c == BACKSPACE)
        {
            if(!stack_empty(&stk_rub) || !stack_empty(&stk_input))
            {
                process_rubout();
            }
        }
        else
        {
            if(stack_empty(&stk_func))
            {
                if(isNumeric(c))
                {
                    process_param_num(c);
                }
                else
                {
                    process_cmd(c);
                }
            }
            else
            {
                if(c == ESCAPE)
                {
                    process_cmd(c);
                }
                else
                {
                    process_param_str(c);
                }
            }
        }
    }
    return status;
}

struct stack_state* parse_get_stk_input()
{
    return &stk_input;
}

struct stack_state* parse_get_stk_gen()
{
    return &stk_gen;
}

struct stack_state* parse_get_stk_func()
{
    return &stk_func;
}

struct stack_state* parse_get_stk_rub()
{
    return &stk_rub;
}

struct stack_state* parse_get_stk_after()
{
    return &stk_after;
}

void parse_set_ecf(cmd_ecf* p_ecf)
{
    ecf = p_ecf;
}

void parse_set_ecf_rub(cmd_rubout* p_ecf_rub)
{
    ecf_rub = p_ecf_rub;
}

cmd_ecf* parse_get_ecf()
{
    return ecf;
}

cmd_rubout* parse_get_ecf_rub()
{
    return ecf_rub;
}

void parse_toggle_sign()
{
    sign = sign==1 ? -1 : 1;
}

// INTERNAL

void reset()
{
    stack_clear(&stk_gen);
    stack_clear(&stk_func);
    stack_clear(&stk_rub);
    stack_clear(&stk_after);
    stack_clear(&stk_input);
    screen_set_prompt(0);
    ecf = ecf_empty;
    ecf_rub = ecf_rub_empty;
    cmd_reset_table();
}

void process_param_num(char c)
{
    stack_push_s(&c, &stk_input);
    screen_input_prompt(c, SCREEN_INPUT_TYPE_TXT);
}

void process_param_str(char c)
{
    stack_push_s(&c, &stk_input);
    screen_input_prompt(c, SCREEN_INPUT_TYPE_TXT);
    ecf(c);
}

void process_rubout()
{
    if(stack_empty(&stk_input))
    {
        cmd_rubout* f;
        stack_pop(&f, &stk_rub);
        f();
        char info;
        stack_pop(&info, &stk_rub);
        int i;
        if(info & MASK_SIGN)
        {
            sign = -1;
        }
        else
        {
            sign = 1;
        }
        if(info & MASK_ARG_CMD)
        {
            stack_pop(&i, &stk_rub);
            arg_given = 1;
            arg_value = i;
        }
        else
        {
            arg_given = 0;
        }
        if(info & MASK_ARG_USER)
        {
            stack_pop(&i, &stk_rub);
            stack_pop(stack_get_base(&stk_input), &stk_rub);
            stack_set_size(i, &stk_input);
        }
    }
    else
    {
        if(!stack_empty(&stk_func))
        {
            ecf_rub();
        }
        stack_pop_s(&stk_input);
    }
    screen_delete_prompt();
}

void process_cmd(char c)
{
    cmd_main* f = cmd_lookup(c);
    if(!f)
    {
        screen_set_msg("unknown cmd");
    }
    else
    {
        screen_set_msg("");
        char info = 0;
        if(sign<0) info |= MASK_SIGN;
        if(arg_given) info |= MASK_ARG_CMD;
        int param = arg_given ? arg_value : sign;
        int arg_user_given = 0;
        if(!stack_empty(&stk_input))
        {
            info |= MASK_ARG_USER;
            arg_user_given = 1;
            if(c != ESCAPE)
            {
                param = sign * atoi(stack_get_base(&stk_input));
            }
            int size = stack_get_size(&stk_input)*sizeof(char);
            stack_push(stack_get_base(&stk_input), size, &stk_rub);
            stack_push(&size, sizeof(int), &stk_rub);
        }
        if(arg_given) stack_push(&arg_value, sizeof(int), &stk_rub);
        stack_push(&info, sizeof(char), &stk_rub);
        
        struct cmd_ret ret = f(arg_given||arg_user_given, param);
        
        switch(ret.ret & CMD_MASK_RET)
        {
            case CMD_RET_SUCCESS:
                sign = 1;
                arg_given = 0;
                stack_clear(&stk_input);
                if(ret.ret & CMD_MASK_VALUE)
                {
                    arg_given = 1;
                    arg_value = ret.value;
                }
                if(c == ESCAPE)
                {
                    screen_input_prompt(ESC_CHAR, SCREEN_INPUT_TYPE_ESC);
                }
                else
                {
                    screen_input_prompt(c, SCREEN_INPUT_TYPE_CMD);
                }
                break;
            case CMD_RET_FAILURE:
                stack_pop_s(&stk_rub);
                if(arg_given) stack_pop_s(&stk_rub);
                if(!stack_empty(&stk_input))
                {
                    stack_pop_s(&stk_rub); // stk_input size
                    stack_pop_s(&stk_rub); // stk_input content
                }
                if(!(ret.ret & CMD_MASK_MSG))
                {
                    screen_set_msg("cmd failed");
                }
                break;
            case CMD_RET_FINISH:
                while(!stack_empty(&stk_after))
                {
                    cmd_after* a;
                    stack_pop(&a, &stk_after);
                    switch(a())
                    {
                        case CMD_RET_FAILURE:
                            status = 2;
                            break;
                        case CMD_RET_EXIT:
                            status = 0;
                            break;
                    }
                }
                reset();
                break;
            case CMD_RET_EXIT:
                status = 0;
                break;
        }
    }
}

