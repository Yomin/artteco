/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "buffer.h"
#include "screen.h"
#include "rubout.h"
#include "exception.h"

#include <string.h>
#include <stdlib.h>

// DEFINES

#define MIN(x,y) ((x)<(y) ? (x) : (y))

struct buffer_line
{
    int status; // line properties
    int offset; // character offset in file line
    int size;   // size of buffer line
};

struct load_state
{
    int counter, numlines, numcolumns;
    struct list_state* lines;
};

// FORWARDS

void load_lines(void* elem, void* akk);

int buffer_write(const char* str, struct buffer_state* buffer);
int buffer_delete(int count, struct buffer_state* buffer);

// EXTERNAL

void buffer_init(const char* name, int number, struct buffer_state* buffer)
{
    strncpy(buffer->name, name, BUFFER_NAME_SIZE-1);
    buffer->name[MIN(strlen(name), BUFFER_NAME_SIZE-1)] = 0;
    buffer->number = number;
    
    list_init(sizeof(struct buffer_line), &buffer->lines);
    struct buffer_line* line = (struct buffer_line*) list_add_sc(&buffer->lines);
    line->status |= BUFFER_STATUS_LAST;

    stack_init(STACK_MODE_SIMPLE, sizeof(char), name, &buffer->stack);
    file_init(&buffer->file);
    buffer->linenumber = 0;
}

void buffer_close(struct buffer_state* buffer)
{
    list_clear(&buffer->lines);
    file_close(&buffer->file);
    stack_finish(&buffer->stack);
}

int buffer_load(const char* file, struct buffer_state* buffer)
{
    switch(file_load(file, &buffer->file))
    {
        case FILE_ERROR_NOT_FOUND:
            return BUFFER_ERROR_FILE_NOT_FOUND;
        case FILE_ERROR_NAME_SIZE:
            return BUFFER_ERROR_FILE_NAME_SIZE;
    }
    
    struct file_chunk* chunk = (struct file_chunk*)list_get(0, &buffer->file.chunks);
    
    struct load_state state;
    state.counter = 0;
    state.numlines = screen_get_buffer_lines();
    state.numcolumns = screen_get_buffer_columns();
    state.lines = &buffer->lines;
    
    ((struct buffer_line*)list_get(0, &buffer->lines))->status = 0;
    list_fold(load_lines, &state, &chunk->lines);
    struct buffer_line* last = (struct buffer_line*) list_last(&buffer->lines);
    last->status |= BUFFER_STATUS_LAST;
    if(state.counter < state.numlines-1)
        last->status &= ~BUFFER_STATUS_CONTINUED;
    
    return 0;
}

void buffer_write_str_rubout()
{
    struct buffer_state* buffer;
    int len;
    rubout_load(&buffer);
    rubout_load(&len);
    buffer_delete(len, buffer);
}

int buffer_write_str(const char* str, struct buffer_state* buffer)
{
    if(buffer_write(str, buffer))
        return -1;
    int len = strlen(str);
    rubout_save(&len, sizeof(int));
    rubout_register(buffer_write_str_rubout, &buffer, sizeof(struct buffer_state*));
    return 0;
}

int buffer_write_char(char c, struct buffer_state* buffer)
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

int buffer_delete_str(int count, struct buffer_state* buffer)
{
    // get characters
    if(buffer_delete(count, buffer))
        return -1;
    rubout_register(buffer_delete_str_rubout, &buffer, sizeof(struct buffer_state*));
    return 0;
}

int buffer_delete_char(struct buffer_state* buffer)
{
    return buffer_delete_str(1, buffer);
}

void buffer_scroll_rubout()
{
    int lines;
    rubout_load(&lines);
    // todo
}

int buffer_scroll(int lines, struct buffer_state* buffer)
{
    // todo
    rubout_register(buffer_scroll_rubout, &lines, sizeof(int));
    return 0;
}

void buffer_display(struct buffer_state* buffer)
{
    screen_clear();
    
    int i, max = screen_get_buffer_lines();
    struct file_chunk* chunk = (struct file_chunk*) list_current(&buffer->file.chunks);
    struct file_line* f_line = (struct file_line*) list_get(buffer->linenumber, &chunk->lines);
    
    struct buffer_line* b_line = (struct buffer_line*) list_get_c(0, &buffer->lines);
    
    for(i=0;i<max;++i)
    {
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
        b_line = (struct buffer_line*) list_next(&buffer->lines);
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

void buffer_register_rubouts()
{
    rubout_ptr_register(buffer_write_str_rubout);
    rubout_ptr_register(buffer_delete_str_rubout);
    rubout_ptr_register(buffer_scroll_rubout);
    rubout_ptr_register(buffer_move_cursor_rubout);
}

// INTERNAL

void load_lines(void* elem, void* akk)
{
    struct load_state* state = (struct load_state*) akk;
    struct file_line* line = (struct file_line*) elem;
    if(!state->lines)
        return;   // already done
    
    struct buffer_line* last = (struct buffer_line*) list_last(state->lines);
    int i = 0, offset = 0, size = last->size;
    
    for(; i < line->size; ++i, ++size)
    {
        if(line->line[i] == '\n' || size == state->numcolumns)
        {
            last->size = size;
            
            if(last->status & BUFFER_STATUS_CONTINUED)
            {
                if(i == 0)
                {
                    last->status &= ~BUFFER_STATUS_CONTINUED;
                    last->status |= BUFFER_STATUS_EXHAUSTED;
                }
            }
            else
                last->offset = offset;
            
            if(line->line[i] == '\n')
            {
                last->status |= BUFFER_STATUS_NEWLINE;
                offset = i+1;   // forget newline
                size = -1;      // "
            }
            else
            {
                offset = i;     // already on next char
                size = 0;       // "
            }
            
            if(state->counter+1 == state->numlines)
            {
                state->lines = 0;   // remember done
                return;
            }
            else
            {
                ++state->counter;
                last = (struct buffer_line*) list_add_s(state->lines);
            }
            
            last->status = 0;
        }
    }
    
    if(offset == i)
        last->status |= BUFFER_STATUS_EXHAUSTED;
    else if(offset < i)
    {
        last->status |= BUFFER_STATUS_CONTINUED;
        last->size = size;
        last->offset = offset;
    }
}

int buffer_write(const char* str, struct buffer_state* buffer)
{
    while(*str)
    {
        // buffer todo
        screen_input_text(*str);
        ++str;
    }
    screen_refresh();
    return 0;
}

int buffer_delete(int count, struct buffer_state* buffer)
{
    while(count > 0)
    {
        // buffer todo
        screen_delete_text();
        --count;
    }
    screen_refresh();
    return 0;
}
