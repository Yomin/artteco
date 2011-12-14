/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "buffer_mgr.h"
#include "list.h"

#include <string.h>

int lines, count_intern, count_buffer;
struct list_state buffer_list;
struct buffer_state* buffer_current;

void buffer_mgr_init(int alines)
{
    lines = alines;
    list_init(sizeof(struct buffer_state), &buffer_list);
    buffer_current = buffer_mgr_add_intern("TECO-Main");
    count_intern = 0;
    count_buffer = 0;
}

void buffer_mgr_finish()
{
    list_clear_f((everyFunc*)buffer_close, &buffer_list);
}

struct buffer_state* add(const char* name, int number, const char* file)
{
    struct buffer_state buf;
    buffer_init(name, number, lines, &buf);
    if(file) if(!buffer_load(file, &buf)) return 0;
    buffer_current = list_add(&buf, &buffer_list);
    return buffer_current;
}

struct buffer_state* buffer_mgr_add(const char* name)
{
    return add(name, ++count_buffer, 0);
}

struct buffer_state* buffer_mgr_add_intern(const char* name)
{
    return add(name, ++count_intern, 0);
}

struct buffer_state* buffer_mgr_add_file(const char* name, const char* file)
{
    return add(name, ++count_buffer, file);
}

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

int buffer_mgr_delete(int number)
{
    if(number == -1)
    {
        return BUFFER_DELETE_FORBIDDEN;
    }
    else if(number == buffer_current->number)
    {
        buffer_mgr_switch(-1);
    }
    return list_remove(match_by_number, &number, &buffer_list);
}

struct buffer_state* buffer_mgr_current()
{
    return buffer_current;
}

struct buffer_state* buffer_mgr_switch(int number)
{
    struct buffer_state* buf = (struct buffer_state*) list_find(match_by_number, &number, &buffer_list);
    if(buf)
    {
        buffer_current = buf;
    }
    return buf;
}
