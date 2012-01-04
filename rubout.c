/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "rubout.h"
#include "stack.h"

// DEFINES

// FORWARDS

// VARIABLES

struct stack_state stk_rubout, stk_counter;
int break_lvl;

// INTERNAL

int stk_counter_inc(void* elem, int size)
{
    char* count = (char*) elem;
    (*count)++;
    return STACK_EXEC_SUCCESS;
}

// EXTERNAL

void rubout_init()
{
    stack_init(STACK_MODE_EXT, sizeof(rubout_func*), "rub", &stk_rubout);
    stack_init(STACK_MODE_SIMPLE, sizeof(char), "counter", &stk_counter);
    stack_set_func(stk_counter_inc, &stk_counter);
    break_lvl = 0;
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
    rubout_func* f;
    
    while(counter-- > 0)
    {
        stack_pop(&f, &stk_rubout);
        f();
    }
}

DEBUGIZE_1(void, rubout_register_s, rubout_func* f)
{
    DEBUG(
        stack_push_s(&f, &stk_rubout),
        stack_push_s_dbg(&f, &stk_rubout, dbg_arg, dbg_func)
    );
    stack_exec(&stk_counter);
    char counter;
    stack_top(&counter, &stk_counter);
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

DEBUGIZE_2(void, rubout_save, void* data, int size)
{
    DEBUG(
        stack_push(data, size, &stk_rubout),
        stack_push_dbg(data, size, &stk_rubout, dbg_arg1, dbg_arg2, dbg_func)
    );
}

int rubout_load(void* data)
{
    return stack_pop(data, &stk_rubout);
}

int rubout_info()
{
    struct stack_elem e = stack_top_p(&stk_rubout);
    return e.size;
}

void rubout_start()
{
    char counter = 0;
    stack_push_s(&counter, &stk_counter);
    if(break_lvl > 0) break_lvl++;
}

void rubout_end()
{
    if(break_lvl != 1)
    {
        char counter;
        stack_pop(&counter, &stk_counter);
        stack_push(&counter, sizeof(char), &stk_rubout);
        if(!stack_empty(&stk_counter))
        {
            stack_push(&rubout, sizeof(rubout_func*), &stk_rubout);
        }
    }
    if(break_lvl > 0) break_lvl--;
}

void rubout_clear()
{
    stack_clear(&stk_rubout);
    stack_clear(&stk_counter);
    break_lvl = 0;
}

void rubout_break()
{
    break_lvl = 1;
}

