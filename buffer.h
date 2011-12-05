/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_BUFFER__
#define __ART_TECO_BUFFER__

#include <stdio.h>

#define BUFFER_LINE_SIZE 1200
#define BUFFER_CHUNK_SIZE 1024*32
#define BUFFER_CHUNK_COUNT 32
#define BUFFER_CHUNK_INACTIVITY 10

struct buffer_line
{
    char line[BUFFER_LINE_SIZE];
    int size;
    struct buffer_line *next, *prev;
};

struct buffer_chunk
{
    int start, end;
    struct buffer_line *lines, *current;
};

struct buffer_state
{
    FILE* file;
    struct buffer_chunk* chunks;
};

struct buffer_state* buffer_init(struct buffer_state* buffer);
//~ struct buffer_state* buffer_load(char* file, struct buffer_state* buffer);
//~ struct buffer_line* buffer_next_line(struct buffer_state* buffer);
//~ struct buffer_line* buffer_prev_line(struct buffer_state* buffer);

#endif
