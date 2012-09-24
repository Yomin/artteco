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

#include "buffer.h"
#include "screen.h"
#include "rubout.h"
#include "exception.h"

#include <string.h>
#include <stdlib.h>

// DEFINES

#define MIN(x,y) ((x)<(y) ? (x) : (y))

#define OP_ADD 0
#define OP_DEL 1
#define OP_MOV 2

struct buffer_line
{
    struct file_pos* pos;
};

struct load_state
{
    int bufferline_counter, numlines, numcolumns;
    struct list_state* lines;
};

struct point
{
    int y, x;
};

// FORWARDS

void load_lines(void* elem, void* akk);
void show_lines(void* elem, void* akk);

void buffer_write(const char* str, struct stack_state* stack);
void buffer_delete(int count, char* buf, struct stack_state* stack);

// EXTERNAL

void buffer_init(const char* name, int number, struct buffer_state* buffer)
{
    strncpy(buffer->name, name, BUFFER_NAME_SIZE-1);
    buffer->name[MIN(strlen(name), BUFFER_NAME_SIZE-1)] = 0;
    buffer->number = number;
    
    file_init(&buffer->file);
    
    list_init(sizeof(struct buffer_line), &buffer->lines);
    struct buffer_line* line = list_add_sc(&buffer->lines);
    line->pos = file_add_pos(0, 0, 0, 0, &buffer->file);

    stack_init(STACK_MODE_EXT|STACK_MODE_QUEUE, sizeof(struct point), name, &buffer->stack);
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
    
    struct file_chunk* chunk = list_get(0, &buffer->file.chunks);
    
    struct load_state state;
    state.bufferline_counter = 0;
    state.numlines = screen_get_buffer_lines();
    state.numcolumns = screen_get_buffer_columns();
    state.lines = &buffer->lines;
    
    list_fold(load_lines, &state, &chunk->lines);
    struct buffer_line* last = list_last(&buffer->lines);
    if(!last->pos)
    {
        list_delete(state.bufferline_counter, &buffer->lines);
        last = list_last(&buffer->lines);
    }
    
    return 0;
}

void buffer_write_str_rubout()
{
    struct buffer_state* buffer;
    int len;
    rubout_load(&buffer);
    rubout_load(&len);
    buffer_delete(len, 0, &buffer->stack);
}

void buffer_write_str(const char* str, struct buffer_state* buffer)
{
    buffer_write(str, &buffer->stack);
    int len = strlen(str);
    rubout_save(&len, sizeof(int));
    rubout_register(buffer_write_str_rubout, &buffer, sizeof(struct buffer_state*));
}

void buffer_write_char(char c, struct buffer_state* buffer)
{
    char str[2];
    str[0] = c;
    str[1] = 0;
    buffer_write_str((const char*)&str, buffer);
}

void buffer_delete_str_rubout()
{
    struct buffer_state* buffer;
    rubout_load(&buffer);
    char* buf = rubout_top_ptr();
    buffer_write_str(buf, buffer);
    rubout_load(0);
}

int buffer_delete_str(int count, struct buffer_state* buffer)
{
    if(count > screen_get_pos()) // todo for scroll
        return BUFFER_ERROR_BEGIN;
    char* buf = rubout_save(0, count+1);
    buffer_delete(count, buf, &buffer->stack);
    buf[count] = 0;
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
    
    int counter = 0;
    list_fold(show_lines, &counter, &buffer->lines);
    
    screen_set_cursor(0, 0);
    screen_refresh();
}

void buffer_move_cursor_rubout()
{
    struct buffer_state* buffer;
    rubout_load(&buffer);
    
    int amount;
    rubout_load(&amount);
    if(amount < 0)
        screen_move_cursor(SCREEN_CURSOR_FORWARD);
    if(amount > 0)
        screen_move_cursor(SCREEN_CURSOR_BACKWARD);
    screen_refresh();
    
    struct point p_first, p_current;
    screen_get_cursor(&p_current.y, &p_current.x);
    
    stack_pop_s(&buffer->stack);         // op
    stack_top(&p_first, &buffer->stack); // pos
    
    if(p_first.y == p_current.y && p_first.x == p_current.x)
    {
        stack_pop_s(&buffer->stack);
    }
    else
        stack_push_vc(OP_MOV, &buffer->stack);
}

int buffer_move_cursor(int amount, struct buffer_state* buffer)
{
    struct point p;
    screen_get_cursor(&p.y, &p.x);
    
    if(amount < 0 && !p.y && !p.x) // todo for scroll
        return BUFFER_ERROR_BEGIN;
    
    // todo forward move
    
    char op;
    int size = stack_pop_e(&op, &buffer->stack);
    if(size >= 0)
    {
        if(op == OP_MOV)
            stack_push_vc(OP_MOV, &buffer->stack);
        else
        {
            stack_push_vc(op, &buffer->stack);
            stack_push_s(&p, &buffer->stack);
            stack_push_vc(OP_MOV, &buffer->stack);
        }
    }
    else
    {
        stack_push_s(&p, &buffer->stack);
        stack_push_vc(OP_MOV, &buffer->stack);
    }
    
    if(amount > 0)
        screen_move_cursor(SCREEN_CURSOR_FORWARD);
    if(amount < 0)
        screen_move_cursor(SCREEN_CURSOR_BACKWARD);
    screen_refresh();
    rubout_save(&amount, sizeof(int));
    rubout_register(buffer_move_cursor_rubout, &buffer, sizeof(struct buffer_state*));
    return 0;
}

void buffer_flush(struct buffer_state* buffer)
{
    int pos_y = 0, pos_x = 0;
    char op;
    int size;
    while((size = stack_pop_e(&op, &buffer->stack)) >= 0)
    {
        switch(op)
        {
            case OP_ADD:
                break;
            case OP_DEL:
                break;
            case OP_MOV:
                break;
        }
    }
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
    struct load_state* state = akk;
    struct file_line* fileline = elem;
    if(!state->lines)
        return;   // already done
    
    struct buffer_line* bufferline = list_last(state->lines);
    int i = 0, offset = 0, size;
    
    if(bufferline->pos) // if existent continuing bufferline
        size = bufferline->pos->size;
    else                // else start value
        size = 0;
    
    for(; i < fileline->size; ++i, ++size)
    {
        if(fileline->line[i] == '\n' || size == state->numcolumns)
        {
            if(bufferline->pos) // if existent continuing bufferline
                bufferline->pos->size = size;
            else                // else create one
                bufferline->pos = file_line_add_pos(size, offset, fileline);
            
            if(fileline->line[i] == '\n')
            {
                offset = i+1;   // forget newline
                size = -1;      // "
            }
            else
            {
                offset = i;     // already on next char
                size = 0;       // "
            }
            
            if(state->bufferline_counter+1 == state->numlines)
            {
                state->lines = 0;   // remember done
                return;
            }
            else
            {
                ++state->bufferline_counter;
                bufferline = list_add_s(state->lines);
            }
        }
    }
    
    if(offset < i) // continuing bufferline, create pos in advance
        bufferline->pos = file_line_add_pos(size, offset, fileline);
}

void show_lines(void* elem, void* akk)
{
    struct buffer_line* b_line = elem;
    struct file_line*   f_line = b_line->pos->line;
    int* counter = akk;
    
    int offset = 0;
    int size = b_line->pos->size;
    char* ptr = &f_line->line[b_line->pos->offset];
    
    if(f_line->size-b_line->pos->offset < size)
    {
        screen_set_line(*counter, ptr);
        offset += f_line->size - b_line->pos->offset;
        size -= offset;
        f_line = f_line->next;
        ptr = f_line->line;
        
        while(size > f_line->size)
        {
            screen_set_line_o(*counter, offset, ptr);
            offset += f_line->size;
            size -= f_line->size;
            f_line = f_line->next;
            ptr = f_line->line;
        }
    }
    
    char tmp = ptr[size];
    ptr[size] = 0;
    screen_set_line_o(*counter, offset, ptr);
    ptr[size] = tmp;
    
    ++(*counter);
}

void buffer_write_new(const char* str, struct stack_state* stack)
{
    stack_push((void*)str, strlen(str)*sizeof(char), stack);
    stack_push_vc(OP_ADD, stack);
}

void buffer_write_concat(const char* str, struct stack_state* stack)
{
    struct stack_elem addbuf = stack_top_p(stack);
    int len = strlen(str);
    stack_top_r(addbuf.size+len, stack);
    memcpy(addbuf.ptr+addbuf.size, str, len);
    stack_push_vc(OP_ADD, stack);
}

void buffer_write_rewind(const char* str, struct stack_state* stack)
{
    struct stack_elem delbuf = stack_top_p(stack);
    char* last = delbuf.ptr+delbuf.size-sizeof(char);
    
    while(last >= delbuf.ptr && *str && *last == *str)
        --last; ++str;
    
    int delrest = last-delbuf.ptr+sizeof(char);
    if(delrest)
    {
        stack_top_r(delrest, stack);
        stack_push_vc(OP_DEL, stack);
        if(*str)
            buffer_write_new(str, stack);
    }
    else
    {
        stack_pop_s(stack);
        if(*str)
        {
            char op;
            int size = stack_pop_e(&op, stack);
            if(size >= 0 && op == OP_ADD)
                buffer_write_concat(str, stack);
            else
                buffer_write_new(str, stack);
        }
    }
}

void buffer_write(const char* str, struct stack_state* stack)
{
    if(!*str)
        return;
    char op;
    int size = stack_pop_e(&op, stack);
    if(size >= 0)
    {
        switch(op)
        {
            case OP_DEL:
                buffer_write_rewind(str, stack);
                break;
            case OP_ADD:
                buffer_write_concat(str, stack);
                break;
            case OP_MOV:
                stack_push_vc(OP_MOV, stack);
                buffer_write_new(str, stack);
                break;
        }
    }
    else
        buffer_write_new(str, stack);
    screen_input_text_sr(str);
}

void buffer_delete_new(int count, char* buf, struct stack_state* stack)
{
    char* elem = (char*) stack_push(0, count*sizeof(char), stack);
    while(count--)
    {
        *elem = screen_delete_text();
        if(buf) buf[count] = *elem++;
    }
    stack_push_vc(OP_DEL, stack);
}

void buffer_delete_concat(int count, char* buf, struct stack_state* stack)
{
    struct stack_elem delbuf = stack_top_p(stack);
    char* elem = (char*) stack_top_r(delbuf.size+count, stack);
    elem += delbuf.size;
    while(count--)
    {
        *elem = screen_delete_text();
        if(buf) buf[count] = *elem++;
    }
    stack_push_vc(OP_DEL, stack);
}

void buffer_delete_rewind(int count, char* buf, struct stack_state* stack)
{
    struct stack_elem added = stack_top_p(stack);
    char* last = added.ptr+added.size-sizeof(char);
    
    char next = screen_get_text(-1);
    while(last >= added.ptr && count && *last == next)
    {
        screen_delete_text();
        --last; --count;
        if(buf) buf[count] = next;
        next = screen_get_text(-1);
    }
    
    int addrest = last-added.ptr+sizeof(char);
    if(addrest)
    {
        stack_top_r(addrest, stack);
        stack_push_vc(OP_ADD, stack);
        if(count)
            buffer_delete_new(count, buf, stack);
    }
    else
    {
        stack_pop_s(stack);
        if(count)
        {
            char op;
            int size = stack_pop_e(&op, stack);
            if(size >= 0 && op == OP_DEL)
                buffer_delete_concat(count, buf, stack);
            else
                buffer_delete_new(count, buf, stack);
        }
    }
}

void buffer_delete(int count, char* buf, struct stack_state* stack)
{
    if(buf) buf[0] = 0;
    if(!count)
        return;
    char op;
    int size = stack_pop_e(&op, stack);
    if(size >= 0)
    {
        switch(op)
        {
            case OP_ADD:
                buffer_delete_rewind(count, buf, stack);
                break;
            case OP_DEL:
                buffer_delete_concat(count, buf, stack);
                break;
            case OP_MOV:
                stack_push_vc(OP_MOV, stack);
                buffer_delete_new(count, buf, stack);
                break;
        }
    }
    else
        buffer_delete_new(count, buf, stack);
    screen_refresh();
}
