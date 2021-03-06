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

/**
 * \def BUFFER_NAME_SIZE
 * \brief Maximal number of chars to name a buffer.
 *
 * \def BUFFER_ERROR_FILE_NOT_FOUND
 * \brief Error: File was not found.
 * \def BUFFER_ERROR_FILE_NAME_SIZE
 * \brief Error: File size exceeded #FILE_NAME_SIZE.
 * \def BUFFER_ERROR_BEGIN
 * \brief Error: Cursor can not move ahead buffer begin.
 * \def BUFFER_ERROR_END
 * \brief Error: Cursor can not move after buffer end.
 */

#define BUFFER_NAME_SIZE 80

#define BUFFER_ERROR_BEGIN            -1
#define BUFFER_ERROR_END              -2
#define BUFFER_ERROR_FILE_NOT_FOUND   -3
#define BUFFER_ERROR_FILE_NAME_SIZE   -4
#define BUFFER_ERROR_FILE_NO_SPACE    -5
#define BUFFER_ERROR_FILE_CANT_WRITE  -6
#define BUFFER_ERROR_FILE_NAME_NEEDED -7
#define BUFFER_ERROR_FILE_SRC_LOST    -8

struct buffer_state
{
    char status;
    char name[BUFFER_NAME_SIZE];
    int number;
    int flush_x, flush_y;
    struct list_state lines;
    struct stack_state stack;
    struct file_state file;
};

void buffer_init(const char* name, int number, struct buffer_state* buffer);
void buffer_close(struct buffer_state* buffer);
int  buffer_load(const char* file, struct buffer_state* buffer);
void buffer_flush(struct buffer_state* buffer);
int  buffer_save(const char* filename, struct buffer_state* buffer);

const char* buffer_status(struct buffer_state* buffer);
void        buffer_display_status(struct buffer_state* buffer, struct buffer_state* prev);

void buffer_write_str(const char* str, struct buffer_state* buffer);
int  buffer_delete_str(int count, struct buffer_state* buffer);
void buffer_write_char(char c, struct buffer_state* buffer);
int  buffer_delete_char(struct buffer_state* buffer);

int  buffer_scroll(int lines, struct buffer_state* buffer);
void buffer_display(struct buffer_state* buffer);
int  buffer_move_cursor(int amount, struct buffer_state* buffer);

void buffer_register_rubouts();

#endif
