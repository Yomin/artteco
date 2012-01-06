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

#define BUFFER_STATUS_NEWLINE   1   // contains newline
#define BUFFER_STATUS_CONTINUED 2   // continues to next file line
#define BUFFER_STATUS_EXHAUSTED 4   // fills rest of file line
#define BUFFER_STATUS_LAST      8   // last line to display

struct buffer_line
{
    int status; // line properties
    int offset; // character offset in file line
    int size;   // size of buffer line
};

struct buffer_state
{
    char name[BUFFER_NAME_SIZE];
    int number, linenumber;
    struct buffer_line* lines;
    struct stack_state stack;
    struct file_state file;
};

struct buffer_state* buffer_init(const char* name, int number, struct buffer_state* buffer);
void buffer_close(struct buffer_state* buffer);
struct buffer_state* buffer_load(const char* file, struct buffer_state* buffer);
struct buffer_state* buffer_write_str(const char* str, struct buffer_state* buffer);
struct buffer_state* buffer_delete_str(int count, struct buffer_state* buffer);
struct buffer_state* buffer_write_char(char c, struct buffer_state* buffer);
struct buffer_state* buffer_delete_char(struct buffer_state* buffer);
struct buffer_state* buffer_scroll(int lines, struct buffer_state* buffer);
void buffer_display(struct buffer_state* buffer);
int buffer_move_cursor(int amount, struct buffer_state* buffer);

void buffer_register_rubouts();

#endif
