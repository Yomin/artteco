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
    stack->ptr = stack->stack;
    stack->stack[0] = 0;
    return stack;
}

struct stack_state* stack_init_s(struct stack_state* stack)
{
    return stack_init(STACK_MODE_EXT, sizeof(char), stack);
}

struct stack_state* stack_push(void* elem, int size, struct stack_state* stack)
{
    int ext = stack->mode == STACK_MODE_EXT;
    int extsize = ext ? sizeof(int) : 0;
    if(stack->ptr+size+extsize-stack->stack >= STACK_SIZE)
    {
        return 0;
    }
    memcpy(stack->ptr, elem, size);
    stack->ptr += size;
    if(ext)
    {
        memcpy(stack->ptr, &size, sizeof(int));
        stack->ptr += sizeof(int);
    }
    *(stack->ptr) = 0;
    return stack;
}

struct stack_state* stack_push_p(void* elemptr, int size, struct stack_state* stack)
{
    void* ptr = elemptr;
    return stack_push(&ptr, size, stack);
}

struct stack_state* stack_push_s(void* elem, struct stack_state* stack)
{
    return stack_push(elem, stack->size, stack);
}

struct stack_state* stack_push_sp(void* elemptr, struct stack_state* stack)
{
    void* ptr = elemptr;
    return stack_push(&ptr, stack->size, stack);
}

struct stack_state* stack_pop(void* dst, struct stack_state* stack)
{
    if(stack->ptr == stack->stack)
    {
        return 0;
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
    return stack;
}

struct stack_state* stack_pop_s(struct stack_state* stack)
{
    return stack_pop(0, stack);
}

struct stack_state* stack_top(void* dst, struct stack_state* stack)
{
    if(stack->ptr == stack->stack)
    {
        return 0;
    }
    int size;
    char* ptr;
    if(stack->mode == STACK_MODE_EXT)
    {
        memcpy(&size, stack->ptr-sizeof(int), sizeof(int));
        ptr = stack->ptr-sizeof(int)-size;
    }
    else
    {
        size = stack->size;
        ptr = stack->ptr-stack->size;
    }
    memcpy(dst, ptr, size);
    return stack;
}

int stack_empty(struct stack_state* stack)
{
    return stack->ptr == stack->stack;
}

void* stack_get_base(struct stack_state* stack)
{
    return stack->stack;
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
