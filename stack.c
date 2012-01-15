/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "stack.h"
#include "exception.h"

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
        return size;
}

#endif

void* push(void* elem, int size, struct stack_state* stack)
{
    int ext = stack->mode == STACK_MODE_EXT;
    int extsize = ext ? sizeof(unsigned char) : 0;
    char* ptr = stack->ptr;
    
    if(stack->ptr+size+extsize-stack->stack >= STACK_SIZE)
        THROW(EXCEPTION_STACK_OVERFLOW);

    if(elem)
        memcpy(stack->ptr, elem, size);
    stack->ptr += size;
    if(ext)
    {
        if(size >= (1 << sizeof(unsigned char)*8))
            THROW(EXCEPTION_OBJECT_SIZE);
        
        unsigned char csize = (unsigned char) size;
        memcpy(stack->ptr, &csize, sizeof(unsigned char));
        stack->ptr += sizeof(unsigned char);
    }
    *(stack->ptr) = 0;
    return ptr;
}

// EXTERNAL

DEBUGIZE_2(void, stack_init, int mode, int size, const char* name, struct stack_state* stack)
{
    stack->mode = (char)mode;
    stack->size = (unsigned char)size;
    stack->base = stack->stack;
    stack->ptr = stack->stack;
    stack->stack[0] = 0;
    #ifndef NDEBUG
        sprintf(buf, "logs/stk_%s", name);
        stack->file = fopen(buf, "w");
        sprintf(stack->type, get_type(dbg_arg2));
    #endif
}

void stack_init_s(const char* name, struct stack_state* stack)
{
    stack_init(STACK_MODE_EXT, sizeof(char), name, stack);
}

void stack_clear(struct stack_state* stack)
{
    stack->ptr = stack->stack;
    *(stack->ptr) = 0;
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
        THROW(EXCEPTION_STACK_EMPTY);
    unsigned char size;
    if(stack->mode == STACK_MODE_EXT)
    {
        memcpy(&size, stack->ptr-sizeof(unsigned char), sizeof(unsigned char));
        stack->ptr -= sizeof(unsigned char)+size;
    }
    else
    {
        size = stack->size;
        stack->ptr -= stack->size;
    }
    if(dst)
        memcpy(dst, stack->ptr, (int)size);
    *(stack->ptr) = 0;
    return (int)size;
}

int stack_pop_s(struct stack_state* stack)
{
    return stack_pop(0, stack);
}

int stack_pop_e(void* dst, struct stack_state* stack)
{
    if(stack_empty(stack)) return -1;
    else return stack_pop(dst, stack);
}

int stack_pop_se(struct stack_state* stack)
{
    if(stack_empty(stack)) return -1;
    else return stack_pop(0, stack);
}

struct stack_elem stack_top_p(struct stack_state* stack)
{
    struct stack_elem elem;
    unsigned char size;
    if(stack->ptr == stack->stack)
        THROW(EXCEPTION_STACK_EMPTY);
    if(stack->mode == STACK_MODE_EXT)
    {
        memcpy(&size, stack->ptr-sizeof(unsigned char), sizeof(unsigned char));
        elem.size = (int)size;
        elem.ptr = stack->ptr-sizeof(unsigned char)-size;
    }
    else
    {
        elem.size = (int)stack->size;
        elem.ptr = stack->ptr-stack->size;
    }
    return elem;
}

int stack_top(void* dst, struct stack_state* stack)
{
    struct stack_elem elem = stack_top_p(stack);
    memcpy(dst, elem.ptr, elem.size);
    return elem.size;
}

int stack_top_e(void* dst, struct stack_state* stack)
{
    if(stack_empty(stack)) return -1;
    else return stack_top(dst, stack);
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

void stack_set_base(void* ptr, struct stack_state* stack)
{
    stack->base = ptr;
}

int stack_get_size(struct stack_state* stack)
{
    return stack->ptr-stack->stack;
}

void stack_set(void* src, int size, struct stack_state* stack)
{
    memcpy(stack->stack, src, size);
    stack->ptr += size;
    *(stack->ptr) = 0;
}

void stack_set_size(int size, struct stack_state* stack)
{
    stack->ptr = stack->stack+size;
}

void stack_set_func(stack_func* f, struct stack_state* stack)
{
    stack->func = f;
}

int stack_exec(struct stack_state* stack)
{
    if(!stack->func)
        THROW(EXCEPTION_FUNCTION_MISSING);
    struct stack_elem elem = stack_top_p(stack);
    return stack->func(elem.ptr, elem.size);
}

int stack_used_bytes(struct stack_state* stack)
{
    return stack->ptr-stack->stack;
}
