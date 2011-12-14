/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_BUFFER__
#define __ART_TECO_BUFFER__

#include "stack.h"
#include "file.h"

#define BUFFER_NAME_SIZE 80

struct line_info
{
    int newline;
    int size;
    char* ptr;
};

struct buffer_state
{
    char name[BUFFER_NAME_SIZE];
    int number;
    struct line_info* lines;
    struct stack_state stack;
    struct file_state file;
};

struct buffer_state* buffer_init(const char* name, int number, int lines, struct buffer_state* buffer);
void buffer_close(struct buffer_state* buffer);
struct buffer_state* buffer_load(const char* file, struct buffer_state* buffer);
struct buffer_state* buffer_write_str(const char* str, struct buffer_state* buffer);
struct buffer_state* buffer_delete_str(int count, struct buffer_state* buffer);
struct buffer_state* buffer_write_char(char c, struct buffer_state* buffer);
struct buffer_state* buffer_delete_char(struct buffer_state* buffer);
struct buffer_state* buffer_scroll(int lines, struct buffer_state* buffer);

#endif
