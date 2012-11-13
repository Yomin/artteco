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
#include "screen.h"
#include "exception.h"

#include <string.h>
#include <libgen.h>

// DEFINES

// FORWARDS

void buffer_mgr_register_rubouts();
int buffer_mgr_map_error(int buffererror);

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
    if(!next)
        return 0;
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
    struct buffer_state* new = list_add(0, &buffer_list);
    buffer_init(name, number, new);
    if(file)
    {
        int ret = buffer_mgr_map_error(buffer_load(file, new));
        if(ret)
        {
            delete(new->number);
            return ret;
        }
    }
    list_find_c(match_by_number, &new->number, &buffer_list);
    buffer_display(new);
    return 0;
}

void mgr_save_rubout()
{
    struct buffer_state* buf;
    rubout_load(&buf);
    rubout_load(&buf->status);
    int num = -1;
    list_remove(match_by_number, &num, &buffer_list);
    --count_buffer;
    buf->number = -1;
    strcpy(buf->name, "TECO-Main");
    buf->file.name[0] = 0;
    if(buf->file.file)
    {
        fclose(buf->file.file);
        buf->file.file = 0;
    }
    screen_set_status(buffer_status(buf));
}

int mgr_save(struct buffer_state* buf, const char* name)
{
    int tmain = buf->number == -1;
    char status = buf->status;
    
    if(tmain)
    {
        struct buffer_state* intern = list_add(0, &buffer_list);
        buffer_init("TECO-Main", -1, intern);
        buf->number = count_buffer++;
    }
    
    int ret = buffer_mgr_map_error(buffer_save(name, buf));
    
    if(tmain)
    {
        if(ret)
        {
            int num = -1;
            list_remove(match_by_number, &num, &buffer_list);
            buf->number = -1;
            --count_buffer;
        }
        else
        {
            rubout_save(&status, sizeof(char));
            rubout_register(mgr_save_rubout, &buf, sizeof(struct buffer_state*));
        }
    }
    
    return ret;
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
    buffer_display_status(list_current(&buffer_list), current);
    rubout_register(mgr_add_rubout, &current->number, sizeof(int));
    return 0;
}

// EXTERNAL

void buffer_mgr_init()
{
    buffer_mgr_register_rubouts();
    list_init_s(sizeof(struct buffer_state), &buffer_list);
    count_intern = -1;
    count_buffer = 1;
    add("TECO-Main", count_intern--, 0);
    screen_set_status(buffer_status(list_current(&buffer_list)));
}

void buffer_mgr_finish()
{
    list_clear_f((mapFunc*)buffer_close, &buffer_list);
}

void buffer_mgr_flush()
{
    list_map((mapFunc*)buffer_flush, &buffer_list);
}

void buffer_mgr_add_rubout()
{
    --count_buffer;
}

int buffer_mgr_add(const char* name)
{
    int ret = mgr_add(name, count_buffer, 0);
    if(!ret)
    {
        ++count_buffer;
        rubout_register_s(buffer_mgr_add_rubout);
    }
    return ret;
}

void buffer_mgr_add_intern_rubout()
{
    ++count_intern;
}

int buffer_mgr_add_intern(const char* name)
{
    int ret = mgr_add(name, count_intern, 0);
    if(!ret)
    {
        --count_intern;
        rubout_register_s(buffer_mgr_add_intern_rubout);
    }
    return ret;
}

int buffer_mgr_add_file(const char* name, const char* file)
{
    int ret = mgr_add(name, count_buffer, file);
    if(!ret)
    {
        ++count_buffer;
        rubout_register_s(buffer_mgr_add_rubout);
    }
    return ret;
}

void buffer_mgr_add_new()
{
    mgr_add(0, count_buffer++, 0);
    rubout_register_s(buffer_mgr_add_rubout);
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
    struct buffer_state* buf = list_get(number, &buffer_list);
    if(!buf)
        return BUFFER_MGR_ERROR_NOT_FOUND;
    char* name = buf->file.name;
    rubout_save(&number, sizeof(int));
    rubout_register(buffer_mgr_delete_rubout, name, strlen(name)*sizeof(char));
    delete(number);
    return 0;
}

int buffer_mgr_save(int number, const char* name)
{
    struct buffer_state* buf = list_get(number, &buffer_list);
    if(!buf)
        return BUFFER_MGR_ERROR_NOT_FOUND;
    return mgr_save(buf, name);
}

int buffer_mgr_save_current(const char* name)
{
    struct buffer_state* buf = list_current(&buffer_list);
    return mgr_save(buf, name);
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
    struct buffer_state* next = switch_buf(number);
    if(!next)
        return 0;
    rubout_register(buffer_mgr_switch_rubout, &current->number, sizeof(int));
    buffer_display_status(next, current);
    return next;
}

// INTERNAL

void buffer_mgr_register_rubouts()
{
    buffer_register_rubouts();
    rubout_ptr_register(mgr_add_rubout);
    rubout_ptr_register(mgr_save_rubout);
    rubout_ptr_register(buffer_mgr_add_rubout);
    rubout_ptr_register(buffer_mgr_add_intern_rubout);
    rubout_ptr_register(buffer_mgr_delete_rubout);
    rubout_ptr_register(buffer_mgr_switch_rubout);
}

int buffer_mgr_map_error(int buffererror)
{
    switch(buffererror)
    {
        case 0:                             return 0;
        case BUFFER_ERROR_FILE_NOT_FOUND:   return BUFFER_MGR_ERROR_FILE_NOT_FOUND;
        case BUFFER_ERROR_FILE_NAME_SIZE:   return BUFFER_MGR_ERROR_FILE_NAME_SIZE;
        case BUFFER_ERROR_FILE_NO_SPACE:    return BUFFER_MGR_ERROR_FILE_NO_SPACE;
        case BUFFER_ERROR_FILE_CANT_WRITE:  return BUFFER_MGR_ERROR_FILE_CANT_WRITE;
        case BUFFER_ERROR_FILE_SRC_LOST:    return BUFFER_MGR_ERROR_FILE_SRC_LOST;
        case BUFFER_ERROR_FILE_NAME_NEEDED: return BUFFER_MGR_ERROR_FILE_NAME_NEEDED;
    }
    THROW(EXCEPTION_UNKNOWN_RETURN);
    return 0;
}
