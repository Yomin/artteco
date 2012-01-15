/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
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
    for(i=0; i<RUBOUT_PTR_COUNT; ++i)
    {
        if(rubout_ptr[i] == f)
            return i;
    }
    return 0;
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
}

void rubout_break()
{
    break_lvl = 1;
}

void rubout_ptr_register(rubout_func* f)
{
    if(rubout_ptr_count == RUBOUT_PTR_COUNT)
        THROW(EXCEPTION_BUFFER_OVERFLOW);
    rubout_ptr[rubout_ptr_count++] = f;
}
