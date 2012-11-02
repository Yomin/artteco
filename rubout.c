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

#include "rubout.h"
#include "stack.h"
#include "exception.h"
#include <stdio.h>

// DEFINES

#define RUBOUT_PTR_COUNT 256
#define RUBOUT_PTR_TYPE  unsigned char

// FORWARDS

// VARIABLES

struct stack_state stk_rubout, stk_counter;
int break_lvl;
rubout_func* rubout_ptr[RUBOUT_PTR_COUNT];
int rubout_ptr_count;
int size_top, size_snd, top_done;

// INTERNAL

int stk_counter_inc(void* elem, int size)
{
    char* count = (char*) elem;
    ++*count;
    return 0;
}

RUBOUT_PTR_TYPE get_intern(rubout_func* f)
{
    RUBOUT_PTR_TYPE i;
    for(i=0; i<rubout_ptr_count; ++i)
    {
        if(rubout_ptr[i] == f)
            return i;
    }
    return 0;
}

void store_top()
{
    if(top_done)
    {
        size_snd = size_top;
        top_done = 0;
        size_top = 0;
    }
    size_top++;
}

// EXTERNAL

void rubout_init()
{
    stack_init(STACK_MODE_EXT, sizeof(RUBOUT_PTR_TYPE), "rub", &stk_rubout);
    stack_init(STACK_MODE_SIMPLE, sizeof(char), "counter", &stk_counter);
    stack_set_func(stk_counter_inc, &stk_counter);
    break_lvl = 0;
    rubout_ptr_count = 1;
    rubout_ptr_register(rubout);
    size_top = size_snd = -1;
    top_done = 1;
}

void rubout_finish()
{
    stack_finish(&stk_rubout);
    stack_finish(&stk_counter);
}

void rubout()
{
    char counter;
    stack_pop(&counter, &stk_rubout);
    RUBOUT_PTR_TYPE g;
    
    while(counter-- > 0)
    {
        stack_pop(&g, &stk_rubout);
        rubout_ptr[g]();
    }
}

DEBUGIZE_1(void, rubout_register_s, rubout_func* f)
{
    RUBOUT_PTR_TYPE g = get_intern(f);
    if(!g)
        THROW(EXCEPTION_FUNCTION_MISSING);
    DEBUG
    (
        stack_push_s(&g, &stk_rubout),
        stack_push_s_dbg(&g, &stk_rubout, dbg_arg, dbg_func)
    );
    store_top();
    top_done = 1;
    stack_exec(&stk_counter);
}

DEBUGIZE_3(void, rubout_register, rubout_func* f, void* data, int size)
{
#ifdef NDEBUG
    rubout_save(data, size);
    rubout_register_s(f);
#else
    rubout_save_dbg(data, size, dbg_arg2, dbg_arg3, dbg_func);
    rubout_register_s_dbg(f, dbg_arg1, dbg_func);
#endif
}

DEBUGIZE_2(void*, rubout_save, void* data, int size)
{
    store_top();
    DEBUG
    (
        return stack_push(data, size, &stk_rubout),
        return stack_push_dbg(data, size, &stk_rubout, dbg_arg1, dbg_arg2, dbg_func)
    );
}

void rubout_load(void* data)
{
    stack_pop(data, &stk_rubout);
}

int rubout_top_size()
{
    struct stack_elem e = stack_top_p(&stk_rubout);
    return e.size;
}

void* rubout_top_ptr()
{
    struct stack_elem e = stack_top_p(&stk_rubout);
    return e.ptr;
}

void rubout_start()
{
    char counter = 0;
    stack_push_s(&counter, &stk_counter);
    if(break_lvl > 0)
        ++break_lvl;
}

void rubout_end()
{
    if(stack_empty(&stk_counter)) // cleared
        return;
    if(break_lvl != 1)
    {
        char counter;
        stack_pop(&counter, &stk_counter);
        stack_push(&counter, sizeof(char), &stk_rubout);
        if(!stack_empty(&stk_counter))
            stack_push_s(&rubout, &stk_rubout);
        else
            DEBUG_LOG_F(stk_rubout.file, "registered %i rubouts, %i bytes used\n",
                counter, stack_used_bytes(&stk_rubout));
    }
    if(break_lvl > 0)
        --break_lvl;
}

void rubout_clear()
{
    stack_clear(&stk_rubout);
    stack_clear(&stk_counter);
    break_lvl = 0;
    rubout_ptr_count = 1;
    size_top = size_snd = -1;
    top_done = 1;
}

void rubout_break()
{
    break_lvl = 1;
}

void rubout_switch_top()
{
    if(size_top == -1 || size_snd == -1)
        THROW(EXCEPTION_FUNCTION_MISSUSE);
    stack_digup(size_top, size_snd, &stk_rubout);
    int tmp = size_top;
    size_top = size_snd;
    size_snd = tmp;
}

void rubout_ptr_register(rubout_func* f)
{
    if(rubout_ptr_count == RUBOUT_PTR_COUNT)
        THROW(EXCEPTION_BUFFER_OVERFLOW);
    rubout_ptr[rubout_ptr_count++] = f;
}
