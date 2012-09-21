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

#ifndef __ART_TECO_FILE__
#define __ART_TECO_FILE__

#include "list.h"

#include <stdio.h>

/**
 * \def FILE_LINE_SIZE
 * \brief Maximal size of lines
 * \def FILE_LINE_COUNT_SOFT
 * \brief Maximal count of lines when loading file.
 * \def FILE_LINE_COUNT_HARD
 * \brief Maximal count of lines per chunk.
 * \def FILE_CHUNK_COUNT
 * \brief Maximal count of chunks per file.
 * \def FILE_NAME_SIZE
 * \brief Maximal size of filenames.
 * 
 * \def FILE_ERROR_NOT_FOUND
 * \brief Error: File was not found.
 * \def FILE_ERROR_NAME_SIZE
 * \brief Error: File size exceeded #FILE_NAME_SIZE.
 */
#define FILE_LINE_SIZE 128
#define FILE_LINE_COUNT_SOFT 64     // up to 8KB
#define FILE_LINE_COUNT_HARD 128    // up to 16KB
#define FILE_CHUNK_COUNT 32         // up to 512KB

#define FILE_NAME_SIZE 80

#define FILE_ERROR_NOT_FOUND -1
#define FILE_ERROR_NAME_SIZE -2

struct file_line
{
    char line[FILE_LINE_SIZE+1];
    int size;
};

struct file_chunk
{
    int start, end;
    struct list_state lines;
};

struct file_state
{
    FILE* file;
    char name[FILE_NAME_SIZE];
    struct list_state chunks;
};

void file_init(struct file_state* file);
void file_close(struct file_state* file);
int  file_load(const char* filename, struct file_state* file);

#endif
