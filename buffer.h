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

#ifndef __ART_TECO_BUFFER__
#define __ART_TECO_BUFFER__

#include "stack.h"
#include "file.h"

#define BUFFER_NAME_SIZE 80

#define BUFFER_STATUS_NEWLINE   1   // contains newline
#define BUFFER_STATUS_CONTINUED 2   // continues to next file line
#define BUFFER_STATUS_EXHAUSTED 4   // fills rest of file line
#define BUFFER_STATUS_LAST      8   // last line to display

#define BUFFER_ERROR_FILE_NOT_FOUND -1
#define BUFFER_ERROR_FILE_NAME_SIZE -2
#define BUFFER_ERROR_BEGIN          -3
#define BUFFER_ERROR_END            -4

struct buffer_state
{
    char name[BUFFER_NAME_SIZE];
    int number, linenumber;
    struct list_state lines;
    struct stack_state stack;
    struct file_state file;
};

void buffer_init(const char* name, int number, struct buffer_state* buffer);
void buffer_close(struct buffer_state* buffer);
int  buffer_load(const char* file, struct buffer_state* buffer);

void buffer_write_str(const char* str, struct buffer_state* buffer);
int  buffer_delete_str(int count, struct buffer_state* buffer);
void buffer_write_char(char c, struct buffer_state* buffer);
int  buffer_delete_char(struct buffer_state* buffer);

int  buffer_scroll(int lines, struct buffer_state* buffer);
void buffer_display(struct buffer_state* buffer);
int  buffer_move_cursor(int amount, struct buffer_state* buffer);

void buffer_flush(struct buffer_state* buffer);

void buffer_register_rubouts();

#endif
