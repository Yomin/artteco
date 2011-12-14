/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\
*/

#include "file.h"

#include <stdlib.h>

struct file_state* file_init(struct file_state* file)
{
    file->chunks = (struct file_chunk*)malloc(sizeof(struct file_chunk));
    file->chunks[0].lines = (struct file_line*)malloc(sizeof(struct file_line));
    file->chunks[0].lines[0].size = 0;
    file->chunks[0].lines[0].next = 0;
    file->chunks[0].lines[0].prev = 0;
    file->chunks[0].current = &file->chunks[0].lines[0];
    file->chunks[0].start = 0;
    file->chunks[0].end = -1;
    file->file = 0;
    return file;
}

struct file_state* file_load(const char* filename, struct file_state* file)
{
    file->file = fopen(filename, "r+");
    // todo
    return file->file ? file : 0;
}
