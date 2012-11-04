/*    _________________________
 *   / _ | _  / /_  __\  _\  _ \
 *  / __ |   / __/ / -_\ \_\ \\ \
 * /_/ |_|_\_\__/_/\___/\___\____\ 
 *
 * Copyright (c) 2012 Martin Rödel aka Yomin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 */

#include "cmd.h"
#include "parse.h"
#include "buffer_mgr.h"
#include "screen.h"
#include "rubout.h"

#include <string.h>
#include <libgen.h>

// DEFINES

#define TAB_STD     0
#define TAB_EXTRA   1
#define TAB_FANCY   2

// FORWARDS

void cmds_register();
void cmd_switch_table_rubout();

// VARIABLES

cmd_main* cmd_table[3][256];
char table_current;

// EXTERNAL

void cmd_init()
{
    rubout_ptr_register(cmd_switch_table_rubout);
    table_current = TAB_STD;
    cmds_register();
}

void cmd_finish()
{
    // nothing to do
}

cmd_main* cmd_lookup(char c)
{
    return cmd_table[(int)table_current][(int)c];
}

void cmd_reset_table()
{
    table_current = TAB_STD;
}

void cmd_switch_table_rubout()
{
    rubout_load(&table_current);
}

void cmd_switch_table(int table)
{
    rubout_register(cmd_switch_table_rubout, &table_current, sizeof(char));
    table_current = (char) table;
}

// INTERNAL

struct cmd_ret ret(int ret, int value)
{
    struct cmd_ret s;
    s.ret = ret;
    s.value = value;
    return s;
}

void cmd_ecf_empty(char c)
{
    // nothing to do
}


// == STD CMDs =================================================================

// ESCAPE
// main:    execute top of func stack, if empty start prompt cleanup

struct cmd_ret std_escape(int given, int param)
{
    if(parse_check_func())
        return ret(CMD_RET_FINISH, 0);
    else
    {
        cmd_func* f = parse_get_func();
        return f(parse_get_param());
    }
}

// MOVE FORWARD
// main:    move cursor given amount forward

struct cmd_ret std_move_forward(int given, int param)
{
    switch(buffer_move_cursor(param, buffer_mgr_current()))
    {
        case BUFFER_ERROR_BEGIN:
            screen_set_msg("buffer begin");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            break;
        case BUFFER_ERROR_END:
            screen_set_msg("buffer end");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            break;
    }
    return ret(CMD_RET_SUCCESS, 0);
}

// MOVE BACKWARD
// main:    move cursor given amount backward

struct cmd_ret std_move_backward(int given, int param)
{
    switch(buffer_move_cursor(-param, buffer_mgr_current()))
    {
        case BUFFER_ERROR_BEGIN:
            screen_set_msg("buffer begin");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            break;
        case BUFFER_ERROR_END:
            screen_set_msg("buffer end");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            break;
    }
    return ret(CMD_RET_SUCCESS, 0);
}

// INSERT
// main:    if param given insert ascii char of param at dot, else register ecf
// ecf:     insert new character at dot
// func[0]: do nothing

void std_insert_ecf(char c)
{
    buffer_write_char(c, buffer_mgr_current());
}

struct cmd_ret std_insert_func(char* str)
{
    parse_register_ecf(cmd_ecf_empty);
    return ret(CMD_RET_SUCCESS, 0);
}

struct cmd_ret std_insert(int given, int param)
{
    if(given)
    {
        if(param < 0 || param > 255)
            return ret(CMD_RET_FAILURE, 0);
        buffer_write_char((char)param, buffer_mgr_current());
    }
    else
    {
        parse_register_func(std_insert_func);
        parse_register_ecf(std_insert_ecf);
    }
    return ret(CMD_RET_SUCCESS, 0);
}

// SUB
// main: toggle sign of param for next cmd

struct cmd_ret std_sub(int given, int param)
{
    if(param > 0)
        parse_toggle_sign();
    return ret(CMD_RET_SUCCESS|CMD_MASK_VALUE, -1);
}

// PUSH
// main:    push param to gen stack

struct cmd_ret std_push(int given, int param)
{
    parse_register_data(&param, sizeof(int));
    return ret(CMD_RET_SUCCESS, 0);
}

// EXTRA
// main: switch cmd table to EXTRA

struct cmd_ret std_extra(int given, int param)
{
    cmd_switch_table(TAB_EXTRA);
    if(given)
        return ret(CMD_RET_SUCCESS|CMD_MASK_VALUE, param);
    else
        return ret(CMD_RET_SUCCESS, 0);
}

// == EXTRA CMDs ===============================================================

// EXIT
// main:        put exit func on after stack
// after[0]:    exit teco

int extra_exit_after()
{
    return CMD_RET_EXIT;
}

struct cmd_ret extra_exit(int given, int param)
{
    parse_register_after(extra_exit_after);
    cmd_switch_table(TAB_STD);
    
    return ret(CMD_RET_SUCCESS, 0);
}

// BUFFER LOAD
// main:    if param given switch buf or show stat else register buffer load function
// func[0]: load file in new buffer

struct cmd_ret extra_buffer_load_func(char* str)
{
    if(strlen(str) == 0)
    {
        screen_set_msg("filename required");
        return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
    }
    else
    {
        switch(buffer_mgr_add_file(basename(str), str))
        {
            case BUFFER_MGR_ERROR_FILE_NOT_FOUND:
                screen_set_msg("file not found");
                return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            case BUFFER_MGR_ERROR_FILE_NAME_SIZE:
                screen_set_msg("filename to long");
                return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        }
        return ret(CMD_RET_SUCCESS, 0);
    }
}

struct cmd_ret extra_buffer_load(int given, int param)
{
    if(given)
    {
        if(param == 0)
        {
            screen_set_msg("not implemented");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        }
        else
        {
            if(!buffer_mgr_switch(param))
            {
                screen_set_msg("buffer not loaded");
                return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
            }
        }
    }
    else
        parse_register_func(extra_buffer_load_func);
    
    cmd_switch_table(TAB_STD);
    
    return ret(CMD_RET_SUCCESS, 0);
}

// BUFFER SAVE
// main:    register buffer save function
// func[0]: flush buffer to file, clear modified bit

struct cmd_ret extra_buffer_save_func(char* str)
{
    if(strlen(str) == 0)
        str = 0;
    
    switch(buffer_save(str, buffer_mgr_current()))
    {
        case BUFFER_ERROR_FILE_NO_SPACE:
            screen_set_msg("no space left");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        case BUFFER_ERROR_FILE_CANT_WRITE:
            screen_set_msg("write failed");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        case BUFFER_ERROR_FILE_NAME_SIZE:
            screen_set_msg("filename to long");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
        case BUFFER_ERROR_FILE_SRC_LOST:
            screen_set_msg("source file not available anymore");
            return ret(CMD_RET_FAILURE|CMD_MASK_MSG, 0);
    }
    
    return ret(CMD_RET_SUCCESS, 0);
}

struct cmd_ret extra_buffer_save(int given, int param)
{
    parse_register_func(extra_buffer_save_func);
    
    cmd_switch_table(TAB_STD);
    
    return ret(CMD_RET_SUCCESS, 0);
}


// REGISTER

void cmds_register()
{
    cmd_table[TAB_STD][27] = std_escape;
    cmd_table[TAB_STD]['c'] = std_move_forward;
    cmd_table[TAB_STD]['e'] = std_extra;
    cmd_table[TAB_STD]['i'] = std_insert;
    cmd_table[TAB_STD]['r'] = std_move_backward;
    cmd_table[TAB_STD]['-'] = std_sub;
    cmd_table[TAB_STD][','] = std_push;
    
    cmd_table[TAB_EXTRA]['b'] = extra_buffer_load;
    cmd_table[TAB_EXTRA]['w'] = extra_buffer_save;
    cmd_table[TAB_EXTRA]['x'] = extra_exit;
}

