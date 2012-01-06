/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
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
struct buffer_state* buffer_current;

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
    struct buffer_state* buf = (struct buffer_state*) list_find(match_by_number, &number, &buffer_list);
    if(buf && buf != buffer_current)
    {
        buffer_current = buf;
        buffer_display(buf);
    }
    return buf;
}

int delete(int number)
{
    if(number == -1)
    {
        return BUFFER_DELETE_FORBIDDEN;
    }
    else if(number == buffer_current->number)
    {
        switch_buf(-1);
    }
    return list_remove(match_by_number, &number, &buffer_list);
}

struct buffer_state* add(const char* name, int number, const char* file)
{
    struct buffer_state buf;
    buffer_init(name, number, &buf);
    if(file) if(!buffer_load(file, &buf)) return 0;
    buffer_current = list_add(&buf, &buffer_list);
    buffer_display(buffer_current);
    return buffer_current;
}

void mgr_add_rubout()
{
    int previous, current = buffer_mgr_current()->number;
    rubout_load(&previous);
    switch_buf(previous);
    delete(current);
}

struct buffer_state* mgr_add(const char* name, int number, const char* file)
{
    int previous = buffer_current->number;
    struct buffer_state* ret = add(name, number, file);
    if(!ret) return 0;
    rubout_register(mgr_add_rubout, &previous, sizeof(int));
    return ret;
}

// EXTERNAL

void buffer_mgr_init()
{
    buffer_mgr_register_rubouts();
    list_init(sizeof(struct buffer_state), &buffer_list);
    count_intern = 0;
    count_buffer = 0;
    buffer_current = add("TECO-Main", --count_intern, 0);
}

void buffer_mgr_finish()
{
    list_clear_f((mapFunc*)buffer_close, &buffer_list);
}

struct buffer_state* buffer_mgr_add(const char* name)
{
    return mgr_add(name, ++count_buffer, 0);
}

struct buffer_state* buffer_mgr_add_intern(const char* name)
{
    return mgr_add(name, --count_intern, 0);
}

struct buffer_state* buffer_mgr_add_file(const char* name, const char* file)
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
    struct buffer_state* buf = (struct buffer_state*) list_get(number, &buffer_list);
    char* name = buf->file.name;
    rubout_save(&number, sizeof(int));
    rubout_register(buffer_mgr_delete_rubout, name, strlen(name)*sizeof(char));
    return delete(number);
}

struct buffer_state* buffer_mgr_current()
{
    return buffer_current;
}

void buffer_mgr_switch_rubout()
{
    int number;
    rubout_load(&number);
    switch_buf(number);
}

struct buffer_state* buffer_mgr_switch(int number)
{
    rubout_register(buffer_mgr_switch_rubout, &buffer_current->number, sizeof(int));
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
