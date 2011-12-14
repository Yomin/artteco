/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\
*/

#ifndef __ART_TECO_FILE__
#define __ART_TECO_FILE__

#include <stdio.h>

#define FILE_LINE_SIZE 1200
#define FILE_CHUNK_SIZE 1024*32
#define FILE_CHUNK_COUNT 32
#define FILE_CHUNK_INACTIVITY 10

struct file_line
{
    char line[FILE_LINE_SIZE];
    int size;
    struct file_line *next, *prev;
};

struct file_chunk
{
    int start, end;
    struct file_line *lines, *current;
};

struct file_state
{
    FILE* file;
    struct file_chunk* chunks;
};

struct file_state* file_init(struct file_state* file);
struct file_state* file_load(const char* filename, struct file_state* file);

#endif
