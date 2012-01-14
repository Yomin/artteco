/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "parse.h"
#include "stack.h"
#include "rubout.h"
#include "help.h"
#include "screen.h"
#include "exception.h"

#include <stdlib.h>

// DEFINES

#define ESCAPE      27
#define BACKSPACE   127
#define ESC_CHAR    '$'

#define SIGN_POSITIV  1
#define SIGN_NEGATIV -1
#define ARG_MISSING   0
#define ARG_AVAILABLE 1

#define STATUS_EXIT    0
#define STATUS_ERROR   1
#define STATUS_SUCCESS -1
#define STATUS_FAILURE -2

// FORWARDS

void ecf_empty(char c) {}

void process_param_num(char c);
void process_param_str(char c);
int  process_cmd(char c);
void parse_register_rubouts();

// VARIABLES

struct stack_state stk_input, stk_func, stk_gen, stk_after;
cmd_ecf* ecf;
char sign;                  // sign for next argument
int arg_mode, arg_value;    // argument from last for next cmd

// EXTERNAL

void parse_init()
{
    parse_register_rubouts();
    
    stack_init(STACK_MODE_SIMPLE, sizeof(char), "input", &stk_input);
    stack_init(STACK_MODE_SIMPLE, sizeof(cmd_func*), "func", &stk_func);
    stack_init(STACK_MODE_SIMPLE, sizeof(cmd_after*), "after", &stk_after);
    
    stack_init(STACK_MODE_EXT, sizeof(int), "gen", &stk_gen);
    
    ecf = ecf_empty;
    
    sign = 1;
    arg_mode = ARG_MISSING;
}

void parse_finish()
{
    stack_finish(&stk_input);
    stack_finish(&stk_gen);
    stack_finish(&stk_func);
    stack_finish(&stk_after);
}

int parse_input(char c)
{
    int ret = STATUS_SUCCESS;
    
    if(c == BACKSPACE)
    {
        if(!stack_empty(&stk_input))
            rubout();
    }
    else
    {
        rubout_start();
        if(stack_empty(&stk_func))
        {
            if(is_numeric(c))
                process_param_num(c);
            else
                ret = process_cmd(c);
        }
        else
        {
            if(c == ESCAPE)
                ret = process_cmd(c);
            else
                process_param_str(c);
        }
        rubout_end();
        if(ret == STATUS_FAILURE)
            rubout();
    }
    return ret;
}

void parse_register_func_rubout()
{
    stack_pop_s(&stk_func);
}

void parse_register_func(cmd_func* f)
{
    stack_push(&f, sizeof(cmd_func*), &stk_func);
    rubout_register_s(parse_register_func_rubout);
}

void parse_register_after_rubout()
{
    stack_pop_s(&stk_after);
}

void parse_register_after(cmd_after* f)
{
    stack_push(&f, sizeof(cmd_after*), &stk_after);
    rubout_register_s(parse_register_after_rubout);
}

void parse_register_ecf_rubout()
{
    rubout_load(&ecf);
}

void parse_register_ecf(cmd_ecf* f)
{
    rubout_register(parse_register_ecf_rubout, &ecf, sizeof(cmd_ecf*));
    ecf = f;
}

void parse_register_data_rubout()
{
    stack_pop_s(&stk_gen);
}

void parse_register_data(void* data, int size)
{
    stack_push(&data, sizeof(cmd_func*), &stk_gen);
    rubout_register_s(parse_register_data_rubout);
}

int parse_check_func()
{
    return stack_empty(&stk_func);
}

int parse_check_after()
{
    return stack_empty(&stk_after);
}

int parse_check_data()
{
    return stack_empty(&stk_gen);
}

void parse_get_func_rubout()
{
    cmd_func* f;
    rubout_load(&f);
    stack_push(&f, sizeof(cmd_func*), &stk_func);
}

cmd_func* parse_get_func()
{
    cmd_func* f;
    stack_pop(&f, &stk_func);
    rubout_register(parse_get_func_rubout, &f, sizeof(cmd_func*));
    return f;
}

void parse_get_after_rubout()
{
    cmd_after* f;
    rubout_load(&f);
    stack_push(&f, sizeof(cmd_after*), &stk_after);
}

cmd_after* parse_get_after()
{
    cmd_after* f;
    stack_pop(&f, &stk_after);
    rubout_register(parse_get_after_rubout, &f, sizeof(cmd_after*));
    return f;
}

char* parse_get_param()
{
    return stack_get_base(&stk_input);
}

void parse_get_data_rubout()
{
    int size = rubout_topsize();
    void* data = stack_push(0, size, &stk_gen);
    rubout_load(data);
}

void parse_get_data(void* dst)
{
    int size = stack_pop(dst, &stk_gen);
    rubout_register(parse_get_data_rubout, dst, size);
}

void parse_toggle_sign_rubout()
{
    rubout_load(&sign);
}

void parse_toggle_sign()
{
    rubout_register(parse_toggle_sign_rubout, &sign, sizeof(char));
    sign = sign==1 ? -1 : 1;
}

// INTERNAL

void set_sign_rubout()
{
    rubout_load(&sign);
}

void set_sign(char mode)
{
    rubout_register(set_sign_rubout, &sign, sizeof(char));
    sign = mode;
}

void set_arg_rubout()
{
    rubout_load(&arg_mode);
    if(arg_mode)
        rubout_load(&arg_value);
}

void set_arg(char mode, int arg)
{
    if(arg_mode)
        rubout_save(&arg_value, sizeof(int));
    rubout_register(set_arg_rubout, &arg_mode, sizeof(char));
    arg_mode = mode;
    arg_value = arg;
}

void set_input_base_rubout()
{
    char* base;
    rubout_load(&base);
    stack_set_base(base, &stk_input);
}

void set_input_base()
{
    char* ptr = stack_get_ptr(&stk_input);
    char* base = stack_get_base(&stk_input);
    stack_set_base(ptr, &stk_input);
    rubout_register(set_input_base_rubout, &base, sizeof(char*));
}

void reset()
{
    stack_clear(&stk_gen);
    stack_clear(&stk_func);
    stack_clear(&stk_after);
    stack_clear(&stk_input);
    screen_reset_prompt();
    ecf = ecf_empty;
    cmd_reset_table();
    rubout_clear();
    arg_mode = ARG_MISSING;
}

void process_param_num_rubout()
{
    stack_pop_s(&stk_input);
    screen_delete_prompt();
}

void process_param_num(char c)
{
    stack_push_s(&c, &stk_input);
    screen_input_prompt(c, SCREEN_INPUT_TYPE_TXT);
    rubout_register_s(process_param_num_rubout);
}

void process_param_str_rubout()
{
    stack_pop_s(&stk_input);
    screen_delete_prompt();
}

void process_param_str(char c)
{
    stack_push_s(&c, &stk_input);
    screen_input_prompt(c, SCREEN_INPUT_TYPE_TXT);
    rubout_register_s(process_param_str_rubout);
    ecf(c);
}

void process_cmd_rubout()
{
    screen_set_msg("");
    screen_delete_prompt();
    stack_pop_s(&stk_input);
}

int process_cmd(char c)
{
    cmd_main* f = cmd_lookup(c);
    if(!f)
    {
        screen_set_msg("unknown cmd");
        return STATUS_FAILURE;
    }
    else
    {
        screen_set_msg("");
        
        int param = arg_mode ? arg_value : sign;
        int arg_given = arg_mode;

        if(!stack_empty_b(&stk_input))
        {
            arg_given = 1;
            if(c != ESCAPE)
            {
                param = sign * atoi(stack_get_base(&stk_input));
            }
        }
        
        set_sign(SIGN_POSITIV);
        
        struct cmd_ret ret = f(arg_given, param);
        
        switch(ret.ret & CMD_MASK_RET)
        {
            case CMD_RET_SUCCESS:
                if(ret.ret & CMD_MASK_VALUE)
                {
                    set_arg(ARG_AVAILABLE, ret.value);
                }
                else
                {
                    set_arg(ARG_MISSING, 0);
                }
                if(c == ESCAPE)
                {
                    screen_input_prompt(ESC_CHAR, SCREEN_INPUT_TYPE_ESC);
                }
                else
                {
                    screen_input_prompt(c, SCREEN_INPUT_TYPE_CMD);
                }
                stack_push_s(&c, &stk_input);
                set_input_base();
                rubout_register_s(process_cmd_rubout);
                return STATUS_SUCCESS;
                
            case CMD_RET_FAILURE:
                if(!(ret.ret & CMD_MASK_MSG))
                {
                    screen_set_msg("cmd failed");
                }
                return STATUS_FAILURE;
            
            case CMD_RET_FINISH:
                while(!stack_empty(&stk_after))
                {
                    cmd_after* a;
                    stack_pop(&a, &stk_after);
                    switch(a())
                    {
                        case CMD_RET_FAILURE:
                            return STATUS_ERROR;
                            break;
                        case CMD_RET_EXIT:
                            return STATUS_EXIT;
                            break;
                    }
                }
                reset();
                break;
                
            case CMD_RET_EXIT:
                return STATUS_EXIT;
        }
    }
    return STATUS_SUCCESS;
}

void parse_register_rubouts()
{
    rubout_ptr_register(parse_register_func_rubout);
    rubout_ptr_register(parse_register_after_rubout);
    rubout_ptr_register(parse_register_ecf_rubout);
    rubout_ptr_register(parse_register_data_rubout);
    rubout_ptr_register(parse_get_func_rubout);
    rubout_ptr_register(parse_get_after_rubout);
    rubout_ptr_register(parse_get_data_rubout);
    rubout_ptr_register(parse_toggle_sign_rubout);
    rubout_ptr_register(set_sign_rubout);
    rubout_ptr_register(set_arg_rubout);
    rubout_ptr_register(set_input_base_rubout);
    rubout_ptr_register(process_param_num_rubout);
    rubout_ptr_register(process_param_str_rubout);
    rubout_ptr_register(process_cmd_rubout);
}
