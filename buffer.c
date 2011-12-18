/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "buffer.h"
#include "screen.h"

#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

// DEFINES

#define min(x,y) (x<y ? x : y)

struct load_state
{
    int counter, lines, columns;
    struct line_info* info;
};

// FORWARDS

void load_lines(void* elem, void* akk);

// EXTERNAL

struct buffer_state* buffer_init(const char* name, int number, int lines, struct buffer_state* buffer)
{
    strncpy(buffer->name, name, BUFFER_NAME_SIZE-1);
    buffer->name[min(strlen(name), BUFFER_NAME_SIZE-1)] = 0;
    buffer->number = number;
    buffer->lines = (struct line_info*) malloc(lines*sizeof(struct line_info));
    memset(buffer->lines, 0, lines*sizeof(struct line_info));
    stack_init(STACK_MODE_SIMPLE, sizeof(char), &buffer->stack);
    file_init(&buffer->file);
    return buffer;
}

void buffer_close(struct buffer_state* buffer)
{
    free(buffer->lines);
    file_close(&buffer->file);
}

struct buffer_state* buffer_load(const char* file, struct buffer_state* buffer)
{
    struct file_state* fs = file_load(file, &buffer->file);
    if(!fs) return 0;
    buffer_display(buffer);
    return buffer;
}

struct buffer_state* buffer_write_str(const char* str, struct buffer_state* buffer)
{
    while(*str)
    {
        // buffer todo
        screen_input_text(*str);
        str++;
    }
    screen_refresh();
    return buffer;
}

struct buffer_state* buffer_write_char(char c, struct buffer_state* buffer)
{
    char str[2];
    str[0] = c;
    str[1] = 0;
    return buffer_write_str((const char*)&str, buffer);
}

struct buffer_state* buffer_delete_str(int count, struct buffer_state* buffer)
{
    while(count > 0)
    {
        // buffer todo
        screen_delete_text();
        count--;
    }
    screen_refresh();
    return buffer;
}

struct buffer_state* buffer_delete_char(struct buffer_state* buffer)
{
    return buffer_delete_str(1, buffer);
}

struct buffer_state* buffer_scroll(int lines, struct buffer_state* buffer)
{
    return buffer;
}

void buffer_display(struct buffer_state* buffer)
{
    screen_clear();
    
    struct file_chunk* chunk = list_get(0, &buffer->file.chunks);
    
    struct load_state state;
    state.counter = 0;
    state.lines = screen_get_lines();
    state.columns = screen_get_columns();
    state.info = buffer->lines;
    
    list_fold(load_lines, &state, &chunk->lines);
    
    screen_set_cursor(0, 0);
    screen_refresh();
}

int buffer_move_cursor(int amount, struct buffer_state* buffer)
{
    // todo
    if(amount > 0) screen_move_cursor(SCREEN_CURSOR_FORWARD);
    if(amount < 0) screen_move_cursor(SCREEN_CURSOR_BACKWARD);
    screen_refresh();
    return 0;
}

// INTERNAL

void load_lines(void* elem, void* akk)
{
    struct load_state* state = (struct load_state*) akk;
    struct file_line* line = (struct file_line*) elem;
    if(state->counter == state->lines) return;
    
    int i = 0, j = state->info[state->counter].size;
    char* start = line->line;
    
    for(; i < line->size; i++, j++)
    {
        if(line->line[i] == '\n' || j == state->columns)
        {
            char tmp = line->line[i];
            line->line[i] = 0;
            screen_set_line(state->counter, start);
            line->line[i] = tmp;
            start = line->line+i;
            
            state->info[state->counter].newline = line->line[i] == '\n';
            state->info[state->counter].size = j;
            state->info[state->counter].ptr = start;
            
            j = 0;
            state->counter++;
            if(state->counter == state->lines) return;
        }
    }
    if(start < line->line+i)
    {
        line->line[i] = 0;
        screen_set_line(state->counter, start);
        state->info[state->counter].newline = 0;
        state->info[state->counter].size = j;
        state->info[state->counter].ptr = start;
    }
}
