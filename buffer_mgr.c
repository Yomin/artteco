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

#include "buffer_mgr.h"
#include "list.h"
#include "rubout.h"

#include <string.h>
#include <libgen.h>

// DEFINES

// FORWARDS

void buffer_mgr_register_rubouts();

// VARIABLES

int count_intern, count_buffer;
struct list_state buffer_list;

// INTERNAL

int match_by_name(void* elem, void* param)
{
    struct buffer_state* buffer = (struct buffer_state*) elem;
    char* name = (char*) param;
    return !strcmp(buffer->name, name);
}

int match_by_number(void* elem, void* param)
{
    struct buffer_state* buffer = (struct buffer_state*) elem;
    int number = *(int*)param;
    return buffer->number == number;
}

struct buffer_state* switch_buf(int number)
{
    struct buffer_state* current = list_current(&buffer_list);
    struct buffer_state* next = list_find_c(match_by_number, &number, &buffer_list);
    if(current != next)
        buffer_display(next);
    return next;
}

void delete(int number)
{
    struct buffer_state* current = list_current(&buffer_list);
    if(number == current->number)
        switch_buf(-1);
    list_remove(match_by_number, &number, &buffer_list);
}

int add(const char* name, int number, const char* file)
{
    struct buffer_state* current = list_add_c(0, &buffer_list);
    buffer_init(name, number, current);
    if(file)
    {
        switch(buffer_load(file, current))
        {
            case BUFFER_ERROR_FILE_NOT_FOUND:
                return BUFFER_MGR_ERROR_FILE_NOT_FOUND;
            case BUFFER_ERROR_FILE_NAME_SIZE:
                return BUFFER_MGR_ERROR_FILE_NAME_SIZE;
        }
    }
    buffer_display(current);
    return 0;
}

void mgr_add_rubout()
{
    int previous, current = buffer_mgr_current()->number;
    rubout_load(&previous);
    switch_buf(previous);
    delete(current);
}

int mgr_add(const char* name, int number, const char* file)
{
    struct buffer_state* buf = list_find_c(match_by_number, &number, &buffer_list);
    if(buf)
        return BUFFER_MGR_ERROR_EXISTING;
    struct buffer_state* current = list_current(&buffer_list);
    int ret = add(name, number, file);
    if(ret)
        return ret;
    rubout_register(mgr_add_rubout, &current->number, sizeof(int));
    return 0;
}

// EXTERNAL

void buffer_mgr_init()
{
    buffer_mgr_register_rubouts();
    list_init(sizeof(struct buffer_state), &buffer_list);
    count_intern = 0;
    count_buffer = 0;
    add("TECO-Main", --count_intern, 0);
}

void buffer_mgr_finish()
{
    list_clear_f((mapFunc*)buffer_close, &buffer_list);
}

void buffer_mgr_add(const char* name)
{
    mgr_add(name, ++count_buffer, 0);
}

void buffer_mgr_add_intern(const char* name)
{
    mgr_add(name, --count_intern, 0);
}

int buffer_mgr_add_file(const char* name, const char* file)
{
    return mgr_add(name, ++count_buffer, file);
}

void buffer_mgr_delete_rubout()
{
    char filename[FILE_NAME_SIZE];
    int number;
    memset(filename, 0, FILE_NAME_SIZE);
    rubout_load(filename);
    rubout_load(&number);
    add(filename, number, basename(filename));
}

int buffer_mgr_delete(int number)
{
    if(number == -1)
        return BUFFER_MGR_ERROR_ACCESS_FORBIDDEN;
    struct buffer_state* buf = (struct buffer_state*) list_get(number, &buffer_list);
    if(!buf)
        return BUFFER_MGR_ERROR_NOT_FOUND;
    char* name = buf->file.name;
    rubout_save(&number, sizeof(int));
    rubout_register(buffer_mgr_delete_rubout, name, strlen(name)*sizeof(char));
    delete(number);
    return 0;
}

struct buffer_state* buffer_mgr_current()
{
    return list_current(&buffer_list);
}

void buffer_mgr_switch_rubout()
{
    int number;
    rubout_load(&number);
    switch_buf(number);
}

struct buffer_state* buffer_mgr_switch(int number)
{
    struct buffer_state* current = list_current(&buffer_list);
    rubout_register(buffer_mgr_switch_rubout, &current->number, sizeof(int));
    return switch_buf(number);
}

// INTERNAL

void buffer_mgr_register_rubouts()
{
    buffer_register_rubouts();
    rubout_ptr_register(mgr_add_rubout);
    rubout_ptr_register(buffer_mgr_delete_rubout);
    rubout_ptr_register(buffer_mgr_switch_rubout);
}
