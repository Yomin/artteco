/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "stack.h"

#include <string.h>

// FORWARDS

// DEFINES

// VARIABLES

#ifndef NDEBUG

char buf[80];

#endif

// INTERNAL

#ifndef NDEBUG

const char* get_type(const char* size)
{
    if(!strncmp(size, "sizeof(", 7))
    {
        strcpy(buf, size+7);
        buf[strlen(buf)-1] = 0;
        return buf;
    }
    else
    {
        return size;
    }
}

#endif

void* push(void* elem, int size, struct stack_state* stack)
{
    int ext = stack->mode == STACK_MODE_EXT;
    int extsize = ext ? sizeof(int) : 0;
    if(stack->ptr+size+extsize-stack->stack >= STACK_SIZE)
    {
        return 0;
    }
    if(elem) memcpy(stack->ptr, elem, size);
    char* ptr = stack->ptr;
    stack->ptr += size;
    if(ext)
    {
        memcpy(stack->ptr, &size, sizeof(int));
        stack->ptr += sizeof(int);
    }
    *(stack->ptr) = 0;
    return ptr;
}

// EXTERNAL

DEBUGIZE_2(struct stack_state*, stack_init, int mode, int size, const char* name, struct stack_state* stack)
{
    stack->mode = mode;
    stack->size = size;
    stack->base = stack->stack;
    stack->ptr = stack->stack;
    stack->stack[0] = 0;
    #ifndef NDEBUG
        sprintf(buf, "logs/stk_%s", name);
        stack->file = fopen(buf, "w");
        sprintf(stack->type, get_type(dbg_arg2));
    #endif
    return stack;
}

struct stack_state* stack_init_s(const char* name, struct stack_state* stack)
{
    return stack_init(STACK_MODE_EXT, sizeof(char), name, stack);
}

struct stack_state* stack_clear(struct stack_state* stack)
{
    stack->ptr = stack->stack;
    *(stack->ptr) = 0;
    return stack;
}

void stack_finish(struct stack_state* stack)
{
    #ifndef NDEBUG
        fclose(stack->file);
    #endif
}

#ifdef NDEBUG

void* stack_push(void* elem, int size, struct stack_state* stack)
{
    return push(elem, size, stack);
}

void* stack_push_p(void* elemptr, int size, struct stack_state* stack)
{
    return push(&elemptr, size, stack);
}

void* stack_push_s(void* elem, struct stack_state* stack)
{
    return push(elem, stack->size, stack);
}

void* stack_push_sp(void* elemptr, struct stack_state* stack)
{
    return push(&elemptr, stack->size, stack);
}

#else

void* stack_push_dbg(void* elem, int size, struct stack_state* stack, const char* dbg_elem, const char* dbg_size, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", get_type(dbg_size), elem, size, dbg_elem);
    return push(elem, size, stack);
}

void* stack_push_p_dbg(void* elemptr, int size, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_size, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", get_type(dbg_size), elemptr, size, dbg_elemptr);
    return push(&elemptr, size, stack);
}

void* stack_push_s_dbg(void* elem, struct stack_state* stack, const char* dbg_elem, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", stack->type, elem, stack->size, dbg_elem);
    return push(elem, stack->size, stack);
}

void* stack_push_sp_dbg(void* elemptr, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", stack->type, elemptr, stack->size, dbg_elemptr);
    return push(&elemptr, stack->size, stack);
}

#endif

int stack_pop(void* dst, struct stack_state* stack)
{
    if(stack->ptr == stack->stack)
    {
        return -1;
    }
    int size;
    if(stack->mode == STACK_MODE_EXT)
    {
        memcpy(&size, stack->ptr-sizeof(int), sizeof(int));
        stack->ptr -= sizeof(int)+size;
    }
    else
    {
        size = stack->size;
        stack->ptr -= stack->size;
    }
    if(dst) memcpy(dst, stack->ptr, size);
    *(stack->ptr) = 0;
    return size;
}

int stack_pop_s(struct stack_state* stack)
{
    return stack_pop(0, stack);
}

struct stack_elem stack_top_p(struct stack_state* stack)
{
    struct stack_elem elem;
    if(stack->ptr == stack->stack)
    {
        elem.ptr = 0;
        return elem;
    }
    if(stack->mode == STACK_MODE_EXT)
    {
        memcpy(&elem.size, stack->ptr-sizeof(int), sizeof(int));
        elem.ptr = stack->ptr-sizeof(int)-elem.size;
    }
    else
    {
        elem.size = stack->size;
        elem.ptr = stack->ptr-stack->size;
    }
    return elem;
}

int stack_top(void* dst, struct stack_state* stack)
{
    struct stack_elem elem = stack_top_p(stack);
    if(!elem.ptr) return -1;
    memcpy(dst, elem.ptr, elem.size);
    return elem.size;
}

int stack_empty(struct stack_state* stack)
{
    return stack->ptr == stack->stack;
}

int stack_empty_b(struct stack_state* stack)
{
    return stack->ptr == stack->base;
}

void* stack_get_base(struct stack_state* stack)
{
    return stack->base;
}

void* stack_get_ptr(struct stack_state* stack)
{
    return stack->ptr;
}

struct stack_state* stack_set_base(void* ptr, struct stack_state* stack)
{
    stack->base = ptr;
    return stack;
}

int stack_get_size(struct stack_state* stack)
{
    return stack->ptr-stack->stack;
}

struct stack_state* stack_set(void* src, int size, struct stack_state* stack)
{
    memcpy(stack->stack, src, size);
    stack->ptr += size;
    *(stack->ptr) = 0;
    return stack;
}

struct stack_state* stack_set_size(int size, struct stack_state* stack)
{
    stack->ptr = stack->stack+size;
    return stack;
}

struct stack_state* stack_set_func(stack_func* f, struct stack_state* stack)
{
    stack->func = f;
    return stack;
}

int stack_exec(struct stack_state* stack)
{
    if(!stack->func) return STACK_EXEC_FAILURE;
    struct stack_elem elem = stack_top_p(stack);
    if(!elem.ptr) return STACK_EXEC_FAILURE;
    return stack->func(elem.ptr, elem.size);
}

