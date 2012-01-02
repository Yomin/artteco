/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\
*/

#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct file_state* file_init(struct file_state* file)
{
    list_init(sizeof(struct file_chunk), &file->chunks);
    struct file_chunk* first_chunk = list_add_s(&file->chunks);
    list_init(sizeof(struct file_line), &first_chunk->lines);
    struct file_line* first_line = list_add_s(&first_chunk->lines);
    first_line->size = 0;
    first_chunk->start = 0;
    first_chunk->end = -1;
    file->file = 0;
    return file;
}

void rem_lines(void* elem)
{
    struct list_state* lines = (struct list_state*) elem;
    list_clear(lines);
}

void file_close(struct file_state* file)
{
    list_clear_f(rem_lines, &file->chunks);
    if(file->file) fclose(file->file);
}

struct file_state* file_load(const char* filename, struct file_state* file)
{
    file->file = fopen(filename, "r+");
    if(!file->file || strlen(filename) > FILE_NAME_SIZE-1) return 0;
    strcpy(file->name, filename);
    int i = 0;
    struct file_chunk* chunk = list_get(0, &file->chunks);
    struct file_line* line = list_get(0, &chunk->lines);
    do
    {
        line->size = fread(line->line, 1, FILE_LINE_SIZE, file->file);
        if(feof(file->file)) break;
        line = list_add_s(&chunk->lines);
        i++;
    }
    while(i < FILE_LINE_COUNT_SOFT);
    return file;
}
