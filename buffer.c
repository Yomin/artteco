/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "buffer.h"
#include "screen.h"
#include "rubout.h"

#include <string.h>
#include <stdlib.h>

// DEFINES

#define MIN(x,y) ((x)<(y) ? (x) : (y))

struct load_state
{
    int counter, numlines, numcolumns;
    struct buffer_line* lines;
};

// FORWARDS

void load_lines(void* elem, void* akk);

struct buffer_state* buffer_write(const char* str, struct buffer_state* buffer);
struct buffer_state* buffer_delete(int count, struct buffer_state* buffer);

// EXTERNAL

struct buffer_state* buffer_init(const char* name, int number, struct buffer_state* buffer)
{
    strncpy(buffer->name, name, BUFFER_NAME_SIZE-1);
    buffer->name[MIN(strlen(name), BUFFER_NAME_SIZE-1)] = 0;
    buffer->number = number;
    
    int lines = screen_get_buffer_lines();
    buffer->lines = (struct buffer_line*) malloc(lines*sizeof(struct buffer_line));
    memset(buffer->lines, 0, lines*sizeof(struct buffer_line));
    buffer->lines[0].status |= BUFFER_STATUS_LAST;

    stack_init(STACK_MODE_SIMPLE, sizeof(char), name, &buffer->stack);
    
    file_init(&buffer->file);
    list_get(0, &buffer->file.chunks); // set current
    buffer->linenumber = 0;
    
    return buffer;
}

void buffer_close(struct buffer_state* buffer)
{
    free(buffer->lines);
    file_close(&buffer->file);
    stack_finish(&buffer->stack);
}

struct buffer_state* buffer_load(const char* file, struct buffer_state* buffer)
{
    struct file_state* fs = file_load(file, &buffer->file);
    if(!fs) return 0;
    
    struct file_chunk* chunk = list_get(0, &buffer->file.chunks);
    
    struct load_state state;
    state.counter = 0;
    state.numlines = screen_get_buffer_lines();
    state.numcolumns = screen_get_buffer_columns();
    state.lines = buffer->lines;
    
    buffer->lines[0].status = 0;
    list_fold(load_lines, &state, &chunk->lines);
    buffer->lines[state.counter].status |= BUFFER_STATUS_LAST;
    if(state.counter < state.numlines-1)
    {
        buffer->lines[state.counter].status &= ~BUFFER_STATUS_CONTINUED;
    }
    
    return buffer;
}

void buffer_write_str_rubout()
{
    struct buffer_state* buffer;
    int len;
    rubout_load(&buffer);
    rubout_load(&len);
    buffer_delete(len, buffer);
}

struct buffer_state* buffer_write_str(const char* str, struct buffer_state* buffer)
{
    int len = strlen(str);
    rubout_save(&len, sizeof(int));
    rubout_register(buffer_write_str_rubout, &buffer, sizeof(struct buffer_state*));
    return buffer_write(str, buffer);
}

struct buffer_state* buffer_write_char(char c, struct buffer_state* buffer)
{
    char str[2];
    str[0] = c;
    str[1] = 0;
    return buffer_write_str((const char*)&str, buffer);
}

void buffer_delete_str_rubout()
{
    struct buffer_state* buffer;
    rubout_load(&buffer);
    // load/insert characters
}

struct buffer_state* buffer_delete_str(int count, struct buffer_state* buffer)
{
    // get characters
    rubout_register(buffer_delete_str_rubout, &buffer, sizeof(struct buffer_state*));
    return buffer_delete(count, buffer);
}

struct buffer_state* buffer_delete_char(struct buffer_state* buffer)
{
    return buffer_delete_str(1, buffer);
}

void buffer_scroll_rubout()
{
    int lines;
    rubout_load(&lines);
    // todo
}

struct buffer_state* buffer_scroll(int lines, struct buffer_state* buffer)
{
    // todo
    rubout_register(buffer_scroll_rubout, &lines, sizeof(int));
    return buffer;
}

void buffer_display(struct buffer_state* buffer)
{
    screen_clear();
    
    int i, max = screen_get_buffer_lines();
    struct file_chunk* chunk = (struct file_chunk*) list_current(&buffer->file.chunks);
    struct file_line* f_line = (struct file_line*) list_get(buffer->linenumber, &chunk->lines);
    
    for(i=0;i<max;i++)
    {
        struct buffer_line* b_line = &buffer->lines[i];
        
        int offset = 0;
        int size = b_line->size;
        char* ptr = &f_line->line[b_line->offset];
        
        if(b_line->status & BUFFER_STATUS_CONTINUED)
        {
            screen_set_line(i, ptr);
            offset = f_line->size - b_line->offset;
            size = b_line->size - offset;
            if(!(f_line = (struct file_line*) list_next(&chunk->lines)))
            {
                chunk = (struct file_chunk*) list_next_s(&buffer->file.chunks);
                f_line = (struct file_line*) list_get(0, &chunk->lines);
            }
            ptr = f_line->line;
        }
        
        char tmp = ptr[size];
        ptr[size] = 0;
        screen_set_line_o(i, offset, ptr);
        ptr[size] = tmp;
        
        if(b_line->status & BUFFER_STATUS_LAST) break;
        
        if(b_line->status & BUFFER_STATUS_EXHAUSTED)
        {
            if(!(f_line = (struct file_line*) list_next(&chunk->lines)))
            {
                chunk = (struct file_chunk*) list_next_s(&buffer->file.chunks);
                f_line = (struct file_line*) list_get(0, &chunk->lines);
            }
        }
    }
    
    screen_set_cursor(0, 0);
    screen_refresh();
}

void buffer_move_cursor_rubout()
{
    int amount;
    rubout_load(&amount);
    if(amount < 0) screen_move_cursor(SCREEN_CURSOR_FORWARD);
    if(amount > 0) screen_move_cursor(SCREEN_CURSOR_BACKWARD);
    screen_refresh();
}

int buffer_move_cursor(int amount, struct buffer_state* buffer)
{
    // todo
    if(amount > 0) screen_move_cursor(SCREEN_CURSOR_FORWARD);
    if(amount < 0) screen_move_cursor(SCREEN_CURSOR_BACKWARD);
    screen_refresh();
    rubout_register(buffer_move_cursor_rubout, &amount, sizeof(int));
    return 0;
}

// INTERNAL

void load_lines(void* elem, void* akk)
{
    struct load_state* state = (struct load_state*) akk;
    struct file_line* line = (struct file_line*) elem;
    if(!state->lines) return;   // already done
    
    int i = 0, offset = 0, size = state->lines[state->counter].size;
    
    for(; i < line->size; i++, size++)
    {
        if(line->line[i] == '\n' || size == state->numcolumns)
        {
            state->lines[state->counter].size = size;
            
            if(state->lines[state->counter].status & BUFFER_STATUS_CONTINUED)
            {
                if(i == 0)
                {
                    state->lines[state->counter].status &= ~BUFFER_STATUS_CONTINUED;
                    state->lines[state->counter].status |= BUFFER_STATUS_EXHAUSTED;
                }
            }
            else
            {
                state->lines[state->counter].offset = offset;
            }
            
            if(line->line[i] == '\n')
            {
                state->lines[state->counter].status |= BUFFER_STATUS_NEWLINE;
                offset = i+1;   // forget newline
                size = -1;      // "
            }
            else
            {
                offset = i;     // already on next char
                size = 0;       // "
            }
            
            state->counter++;
            if(state->counter == state->numlines)
            {
                state->counter--;   // ptr to last
                state->lines = 0;   // remember done
                return;
            }
            
            state->lines[state->counter].status = 0;
        }
    }
    
    if(offset == i)
    {
        state->lines[state->counter].status |= BUFFER_STATUS_EXHAUSTED;
    }
    else if(offset < i)
    {
        state->lines[state->counter].status |= BUFFER_STATUS_CONTINUED;
        state->lines[state->counter].size = size;
        state->lines[state->counter].offset = offset;
    }
}

struct buffer_state* buffer_write(const char* str, struct buffer_state* buffer)
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

struct buffer_state* buffer_delete(int count, struct buffer_state* buffer)
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
