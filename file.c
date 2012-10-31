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

#define INSERT_PREV 0
#define INSERT_NEXT 1

#define LINE_START(Line) (Line->line + Line->start)
#define LINE_END(Line)   (Line->line + Line->start + Line->size)
#define LINE_FREE(Line)  (FILE_LINE_SIZE - Line->start - Line->size)

// VARIABLES

// INTERNAL

void rem_lines(void* elem)
{
    struct list_state* lines = elem;
    list_clear(lines);
}

// EXTERNAL

/** \brief Initialize file structure.
 * 
 * Initialize file structure by initializing the chunk list, adding one chunk,
 * initializing the chunks line list and adding one line.
 * 
 * \param [out] file file structure to be initialized
 */
void file_init(struct file_state* file)
{
    list_init_s(sizeof(struct file_chunk), &file->chunks);
    struct file_chunk* first_chunk = list_add_sc(&file->chunks);
    list_init(LIST_MODE_BASE, sizeof(struct file_line), &first_chunk->lines);
    struct file_line* first_line = list_add_sc(&first_chunk->lines);
    list_init_s(sizeof(struct file_pos), &first_line->pos);
    first_line->size = 0;
    list_init_s(sizeof(struct file_pos), &first_line->pos);
    first_chunk->start = 0;
    first_chunk->end = -1;
    file->file = 0;
}

/** \brief Close file.
 * 
 * Close file by freeing all chunks and the respective lines, then closing
 * the file.
 * 
 * \param [in] file file structure of file to be closed
 */
void file_close(struct file_state* file)
{
    list_clear_f(rem_lines, &file->chunks);
    if(file->file)
        fclose(file->file);
}

/** \brief Open a file.
 * 
 * Open a file by filling a chunk with #FILE_LINE_COUNT_SOFT number
 * of #FILE_LINE_SIZE sized lines read from the file. The rest of the file
 * is loaded on demand.
 * 
 * \param [in]     filename      file name
 * \param [in,out] file          file structure containing chunklist
 * \retval #FILE_ERROR_NOT_FOUND file was not found
 * \retval #FILE_ERROR_NAME_SIZE file name exceeded allowed size
 * \retval 0                     file successful opened
 */
int file_load(const char* filename, struct file_state* file)
{
    file->file = fopen(filename, "r+");
    if(!file->file)
        return FILE_ERROR_NOT_FOUND;
    if(strlen(filename) > FILE_NAME_SIZE-1)
        return FILE_ERROR_NAME_SIZE;
    
    strcpy(file->name, filename);
    int i = 0;
    struct file_chunk* chunk = list_get(0, &file->chunks);
    struct file_line* line = list_get(0, &chunk->lines);

    do
    {
        line->size = fread(line->line, 1, FILE_LINE_SIZE, file->file);
        if(feof(file->file))
            break;
        if(ferror(file->file))
            THROW(EXCEPTION_IO);
        line = list_add_s(&chunk->lines);
        list_init_s(sizeof(struct file_pos), &line->pos);
        ++i;
    }
    while(i < FILE_LINE_COUNT_SOFT);
    return 0;
}

/** \brief Add a #file_pos to a line.
 * 
 * Similar version of #file_line_add_pos.
 * Instead of providing a #file_line a #file_state with line and chunk number
 * is needed.
 * 
 * \param line  line number the #file_pos is added to
 * \param chunk number of the chunk containing the #file_line
 * \param file  #file_state containing the #file_chunk and #file_line
 */
struct file_pos* file_add_pos(int chunk, int line, int size, int offset, struct file_state* file)
{
    struct file_chunk* first = list_get(chunk, &file->chunks);
    return file_line_add_pos(size, offset, list_get(line, &first->lines));
}

/** \brief Add a #file_pos to a line.
 * 
 * Similar version of #file_line_add_pos.
 * Instead of providing a #file_line a #file_chunk with line number is needed.
 * 
 * \param line  line number the #file_pos is added to
 * \param chunk #file_chunk containing the #file_line
 */
struct file_pos* file_chunk_add_pos(int line, int size, int offset, struct file_chunk* chunk)
{
    return file_line_add_pos(size, offset, list_get(line, &chunk->lines));
}

/** \brief Add a #file_pos to a line.
 * 
 * Add a #file_pos to the line pointed to by line, set size offset and self
 * and return a pointer to the created #file_pos.
 * 
 * \param size   size of string the position points to
 * \param offset offset of position in line
 * \param line   #file_line the #file_pos is added to
 * \return returns a pointer to the created #file_pos
 */
struct file_pos* file_line_add_pos(int size, int offset, struct file_line* line)
{
    struct file_pos* pos = list_add_s(&line->pos);
    pos->offset = offset;
    pos->size = size;
    pos->line = line;
    pos->newline = 0;
    return pos;
}

/** \brief Check if enough characters available.
 * 
 * Check the chained lines on availability of a certain amount of characters.
 * This can be used to determine if enough characters for cursor movement
 * are available.
 * 
 * \param amount amount of characters to have be available
 * \param offset offset in line
 * \param line   #file_line the check begins with
 * \retval 0     sufficient characters available
 * \retval -1    unsufficient characters available
 */
int file_check_sufficient(int amount, int offset, struct file_line* line)
{
    if(offset+amount > line->size)
    {
        offset -= line->size;
        line = list_get_next_d(line, sizeof(struct file_line));
        if(!line)
            return -1;
    }
    else if(offset+amount < 0)
    {
        line = list_get_prev_d(line, sizeof(struct file_line));
        if(!line)
            return -1;
        offset = line->size - offset;
    }
    
    if(amount < 0)
    {
        amount += offset;
        line = list_get_prev_d(line, sizeof(struct file_line));
        while(line && amount < 0)
        {
            amount += line->size;
            line = list_get_prev_d(line, sizeof(struct file_line));
        }
        if(amount >= 0)
            return 0;
        return -1;
    }
    else
    {
        amount -= line->size-offset;
        line = list_get_next_d(line, sizeof(struct file_line));
        while(line && amount > 0)
        {
            amount -= line->size;
            line = list_get_next_d(line, sizeof(struct file_line));
        }
        if(amount <= 0)
            return 0;
        return -1;
    }
}

/** \brief Insert string into #file_line.
 * 
 * 
 * 
 * \param str    string to insert
 * \param size   size of string to insert
 * \param offset offset of insert position in #file_line
 * \param line   #file_line
 */
void file_insert(const char *str, int size, int offset, struct file_line* line)
{
    struct list_state* lines = list_get_base_d(line, sizeof(struct file_line));
    
    list_pos_c(line, lines);
    
    if(offset <= line->size/2)
    {
        if(size <= line->start)
        {
            memmove(LINE_START(line)-size, LINE_START(line), offset);
            line->start -= size;
            memcpy(LINE_START(line)+offset, str, size);
            line->size += size;
        }
        else
        {
            struct file_line* prev = list_prev_s(lines);
            
            if(size > LINE_FREE(prev) + line->start)
                prev = list_insert_prev(0, lines);
            
            if(size < offset + line->start)
            {
                int rest = line->start+offset-size;
                memcpy(LINE_END(prev), LINE_START(line), offset-rest);
                prev->size += offset-rest;
                memcpy(line->line, LINE_START(line) + offset-rest, rest);
                memcpy(line->line+rest, str, size);
                line->size += line->start;
                line->start = 0;
            }
            else
            {
                memcpy(LINE_END(prev), LINE_START(line), offset);
                prev->size += offset;
                int rest = size-offset-line->start;
                int avail = LINE_FREE(prev);
                
                if(rest <= avail)
                {
                    memcpy(LINE_END(prev), str, rest);
                    prev->size += rest;
                    memcpy(line->line, str+rest, offset+line->start);
                    line->size += line->start;
                    line->start = 0;
                }
                else
                {
                    memcpy(LINE_END(prev), str, avail);
                    prev->size += avail;
                    str += avail;
                    rest -= avail;
                    
                    while(rest > offset+line->start)
                    {
                        if(rest <= FILE_LINE_SIZE)
                            avail = rest;
                        else
                            avail = FILE_LINE_SIZE;
                        prev = list_insert_prev(0, lines);
                        memcpy(prev->line, str, avail);
                        prev->size = avail;
                        str += avail;
                        rest -= avail;
                    }
                    line->start += offset-rest;
                    line->size += rest-offset;
                    memcpy(LINE_START(line), str, rest);
                }
            }
        }
    }
    else
    {
        if(size <= FILE_LINE_SIZE - line->start - line->size)
        {
            memmove(LINE_START(line)+offset+size, LINE_START(line)+offset, line->size-offset);
            memcpy(LINE_START(line)+offset, str, size);
            line->size += size;
        }
        else
        {
            struct file_line* next = list_next_s(lines);
            
            int avail = FILE_LINE_SIZE - line->start - offset;
            int rest = line->size-offset;
            int splitlen = rest - (avail-size);
            
            if(size > LINE_FREE(line) + next->start)
            {
                next = list_insert_next_c(0, lines);
                next->start = splitlen;
            }
            
            if(size < avail)
            {
                memcpy(LINE_START(next)-splitlen, LINE_START(line)+offset+(avail-size), splitlen);
                next->start -= splitlen;
                next->size += splitlen;
                memmove(LINE_START(line)+offset+size, LINE_START(line)+offset, (avail-size));
                memcpy(LINE_START(line)+offset, str, size);
                line->size += LINE_FREE(line);
            }
            else
            {
                rest = line->size-offset;
                memcpy(LINE_START(next)-offset, LINE_START(line)+offset, rest);
                next->start -= rest;
                next->size += rest;
                line->size -= rest;
                
                rest = size-next->start;
                avail = LINE_FREE(line);
                
                if(rest <= avail)
                {
                    memcpy(LINE_END(line), str, rest);
                    line->size += rest;
                    memcpy(next->line, str+rest, next->start);
                    next->size += next->start;
                    next->start = 0;
                }
                else
                {
                    memcpy(LINE_END(line), str, avail);
                    line->size += avail;
                    str += avail;
                    rest -= avail;
                    
                    while(rest > next->start)
                    {
                        if(rest <= FILE_LINE_SIZE)
                            avail = rest;
                        else
                            avail = FILE_LINE_SIZE;
                        line = list_insert_next_c(0, lines);
                        memcpy(line->line, str, avail);
                        line->size = avail;
                        str += avail;
                        rest -= avail;
                    }
                    next->start -= rest;
                    line->size += rest;
                    memcpy(LINE_START(next), str, rest);
                }
            }
        }
    }
}

/** \brief Erase string from #file_line.
 * 
 * 
 * 
 * \param size   size of string to erase
 * \param offset offset of erase position in #file_line
 * \param line   #file_line
 */
void file_erase(int size, int offset, struct file_line* line)
{
    
}

