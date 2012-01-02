/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "stack.h"
#include <string.h>

struct stack_state* stack_init(int mode, int size, struct stack_state* stack)
{
    stack->mode = mode;
    stack->size = size;
    stack->base = stack->stack;
    stack->ptr = stack->stack;
    stack->stack[0] = 0;
    return stack;
}

struct stack_state* stack_init_s(struct stack_state* stack)
{
    return stack_init(STACK_MODE_EXT, sizeof(char), stack);
}

void* stack_push(void* elem, int size, struct stack_state* stack)
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

void* stack_push_p(void* elemptr, int size, struct stack_state* stack)
{
    return stack_push(&elemptr, size, stack);
}

void* stack_push_s(void* elem, struct stack_state* stack)
{
    return stack_push(elem, stack->size, stack);
}

void* stack_push_sp(void* elemptr, struct stack_state* stack)
{
    return stack_push(&elemptr, stack->size, stack);
}

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

struct stack_state* stack_pop_s(struct stack_state* stack)
{
    if(stack_pop(0, stack) >= 0) return stack;
    else return 0;
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

struct stack_state* stack_clear(struct stack_state* stack)
{
    stack->ptr = stack->stack;
    *(stack->ptr) = 0;
    return stack;
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

