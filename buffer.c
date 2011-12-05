/*    _______________________
//   / _ / _ / /_  __\  _\   \
//  / _ /   / __/ / -_\ \_\ \ \
// /_/_/_/\_\__/_/\___/\___\___\ 
*/

#include "buffer.h"
#include <stdlib.h>

struct buffer_state* buffer_init(struct buffer_state* buffer)
{
    buffer->chunks = (struct buffer_chunk*)malloc(sizeof(struct buffer_chunk));
    buffer->chunks->lines = (struct buffer_line*)malloc(sizeof(struct buffer_line));
    buffer->chunks->lines->size = 0;
    buffer->chunks->lines->next = 0;
    buffer->chunks->lines->prev = 0;
    buffer->chunks->current = buffer->chunks->lines;
    buffer->chunks->start = 0;
    buffer->chunks->end = -1;
    buffer->file = 0;
    return buffer;
}

//~ struct buffer_state* buffer_load(char* file, struct buffer_state* buffer)
//~ {
    //~ buffer->file = fopen(file, "rw");
    //~ if(!buffer->file) return 0;
    //~ return buffer;
//~ }
//~ 
//~ struct buffer_line* buffer_next_line(struct buffer_state* buffer)
//~ {
    //~ 
//~ }
//~ 
//~ struct buffer_line* buffer_prev_line(struct buffer_state* buffer)
//~ {
    //~ 
//~ }
