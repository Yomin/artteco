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

#include "file.h"
#include "exception.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// FORWARDS

// DEFINES

// VARIABLES

// INTERNAL

void rem_lines(void* elem)
{
    struct list_state* lines = (struct list_state*) elem;
    list_clear(lines);
}

// EXTERNAL

void file_init(struct file_state* file)
{
    list_init(sizeof(struct file_chunk), &file->chunks);
    struct file_chunk* first_chunk = (struct file_chunk*) list_add_sc(&file->chunks);
    list_init(sizeof(struct file_line), &first_chunk->lines);
    struct file_line* first_line = (struct file_line*) list_add_sc(&first_chunk->lines);
    first_line->size = 0;
    first_chunk->start = 0;
    first_chunk->end = -1;
    file->file = 0;
}

void file_close(struct file_state* file)
{
    list_clear_f(rem_lines, &file->chunks);
    if(file->file)
        fclose(file->file);
}

int file_load(const char* filename, struct file_state* file)
{
    file->file = fopen(filename, "r+");
    if(!file->file)
        return FILE_ERROR_NOT_FOUND;
    if(strlen(filename) > FILE_NAME_SIZE-1)
        return FILE_ERROR_NAME_SIZE;
    
    strcpy(file->name, filename);
    int i = 0;
    struct file_chunk* chunk = (struct file_chunk*) list_get(0, &file->chunks);
    struct file_line* line = (struct file_line*) list_get(0, &chunk->lines);

    do
    {
        line->size = fread(line->line, 1, FILE_LINE_SIZE, file->file);
        if(feof(file->file))
            break;
        if(ferror(file->file))
            THROW(EXCEPTION_IO);
        line = (struct file_line*) list_add_s(&chunk->lines);
        ++i;
    }
    while(i < FILE_LINE_COUNT_SOFT);
    return 0;
}
