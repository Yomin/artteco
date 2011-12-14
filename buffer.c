/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "buffer.h"
#include "screen.h"

#include <string.h>
#include <stdlib.h>

#define min(x,y) (x<y ? x : y)

struct buffer_state* buffer_init(const char* name, int number, int lines, struct buffer_state* buffer)
{
    strncpy(buffer->name, name, BUFFER_NAME_SIZE-1);
    buffer->name[min(strlen(name), BUFFER_NAME_SIZE-1)] = 0;
    buffer->number = number;
    buffer->lines = (struct line_info*) malloc(lines*sizeof(struct line_info));
    stack_init(STACK_MODE_SIMPLE, sizeof(char), &buffer->stack);
    file_init(&buffer->file);
    return buffer;
}

void buffer_close(struct buffer_state* buffer)
{
    free(buffer->lines);
}

struct buffer_state* buffer_load(const char* file, struct buffer_state* buffer)
{
    struct file_state* fs = file_load(file, &buffer->file);
    // todo
    return fs ? buffer : 0;
}

struct buffer_state* buffer_write_str(const char* str, struct buffer_state* buffer)
{
    while(*str)
    {
        // buffer todo
        screen_input_text(*str);
        str++;
    }
    return buffer;
}

struct buffer_state* buffer_write_char(char c, struct buffer_state* buffer)
{
    char str[2];
    str[0] = c;
    str[1] = 0;
    return buffer_write_str((const char*)&str, buffer);
}

struct buffer_state* buffer_delete_str(int count, struct buffer_state* buffer)
{
    while(count > 0)
    {
        // buffer todo
        screen_delete_text();
        count--;
    }
    return buffer;
}

struct buffer_state* buffer_delete_char(struct buffer_state* buffer)
{
    return buffer_delete_str(1, buffer);
}

struct buffer_state* buffer_scroll(int lines, struct buffer_state* buffer)
{
    return buffer;
}
