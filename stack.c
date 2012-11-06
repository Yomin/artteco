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

#include "stack.h"
#include "exception.h"

#include <string.h>
#include <stdlib.h>

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
    if(size >= (1 << sizeof(unsigned char)*8))
        THROW(EXCEPTION_OBJECT_SIZE);
    unsigned char csize = (unsigned char) size;
    
    int ext = stack->mode & STACK_MODE_EXT ? 1 : 0;
    int queue = stack->mode & STACK_MODE_QUEUE ? 1 : 0;
    int extrasize = (queue+1) * ext * sizeof(unsigned char);
    
    if(stack->ptr+size+extrasize-stack->stack >= STACK_SIZE)
        THROW(EXCEPTION_STACK_OVERFLOW);
    
    if(ext && queue)
    {
        memcpy(stack->ptr, &csize, sizeof(unsigned char));
        stack->ptr += sizeof(unsigned char);
    }
    
    char* ptr = stack->ptr;
    if(elem)
        memcpy(stack->ptr, elem, size);
    stack->ptr += size;
    
    if(ext)
    {
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
    stack->queue = stack->stack;
    stack->stack[0] = 0;
    #ifndef NDEBUG
        sprintf(buf, "logs/stk_%s", name);
        stack->file = fopen(buf, "w");
        sprintf(stack->type, get_type(dbg_arg2));
    #endif
}

void stack_init_s(const char* name, struct stack_state* stack)
{
    stack_init(STACK_MODE_SIMPLE, sizeof(char), name, stack);
}

void stack_clear(struct stack_state* stack)
{
    stack->ptr = stack->stack;
    stack->base = stack->stack;
    stack->queue = stack->stack;
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

void* stack_push_p(void* elemptr, struct stack_state* stack)
{
    return push(&elemptr, sizeof(void*), stack);
}

void* stack_push_v(long value, int size, struct stack_state* stack)
{
    return push((&value)+sizeof(long)-size, size, stack);
}

void* stack_push_vi(int value, struct stack_state* stack)
{
    return push(&value, sizeof(int), stack);
}

void* stack_push_vc(char value, struct stack_state* stack)
{
    return push(&value, sizeof(char), stack);
}

void* stack_push_s(void* elem, struct stack_state* stack)
{
    return push(elem, stack->size, stack);
}

#else

void* stack_push_dbg(void* elem, int size, struct stack_state* stack, const char* dbg_elem, const char* dbg_size, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", get_type(dbg_size), elem, size, dbg_elem);
    return push(elem, size, stack);
}

void* stack_push_p_dbg(void* elemptr, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "void* %p %i %s", elemptr, sizeof(void*), dbg_elemptr);
    return push(&elemptr, sizeof(void*), stack);
}

void* stack_push_v_dbg(long value, int size, struct stack_state* stack, const char* dbg_value, const char* dbg_size, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %i %i %s", get_type(dbg_size), value, size, dbg_value);
    return push((&value)+sizeof(long)-size, size, stack);
}

void* stack_push_vi_dbg(int value, struct stack_state* stack, const char* dbg_value, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "int %i %i %s", value, sizeof(int), dbg_value);
    return push(&value, sizeof(int), stack);
}

void* stack_push_vc_dbg(char value, struct stack_state* stack, const char* dbg_value, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "int %i %i %s", value, sizeof(char), dbg_value);
    return push(&value, sizeof(char), stack);
}

void* stack_push_s_dbg(void* elem, struct stack_state* stack, const char* dbg_elem, const char* dbg_func)
{
    debug_log_f(dbg_func, stack->file, "%s %p %i %s", stack->type, elem, stack->size, dbg_elem);
    return push(elem, stack->size, stack);
}

#endif

int stack_pop(void* dst, struct stack_state* stack)
{
    if(stack->ptr == stack->stack)
        THROW(EXCEPTION_STACK_EMPTY);
    unsigned char size;
    if(stack->mode & STACK_MODE_EXT)
    {
        memcpy(&size, stack->ptr-sizeof(unsigned char), sizeof(unsigned char));
        stack->ptr -= sizeof(unsigned char)+size;
        if(dst)
            memcpy(dst, stack->ptr, (int)size);
        if(stack->mode & STACK_MODE_QUEUE)
            stack->ptr -= sizeof(unsigned char);
    }
    else
    {
        size = stack->size;
        stack->ptr -= stack->size;
        if(dst)
            memcpy(dst, stack->ptr, (int)size);
    }
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
    if(stack->mode & STACK_MODE_EXT)
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

void* stack_top_r(int size, struct stack_state* stack)
{
    if(stack->ptr == stack->stack)
        THROW(EXCEPTION_STACK_EMPTY);
    if(!(stack->mode & STACK_MODE_EXT))
        THROW(EXCEPTION_WRONG_CONFIG);
    
    unsigned char nsize = (unsigned char) size;
    unsigned char osize;
    memcpy(&osize, stack->ptr-sizeof(unsigned char), sizeof(unsigned char));
    stack->ptr = stack->ptr-osize+nsize;
    memcpy(stack->ptr-sizeof(unsigned char), &nsize, sizeof(unsigned char));
    if(stack->mode & STACK_MODE_QUEUE)
        memcpy(stack->ptr-nsize-2*sizeof(unsigned char), &nsize, sizeof(unsigned char));
    return stack->ptr-nsize-sizeof(unsigned char);
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

int stack_queue_get(void* elem, struct stack_state* stack)
{
    if(!(stack->mode & STACK_MODE_QUEUE))
        THROW(EXCEPTION_WRONG_CONFIG);
    if(stack->queue == stack->ptr)
        THROW(EXCEPTION_QUEUE_EMPTY);
    
    unsigned char size;
    if(stack->mode & STACK_MODE_EXT)
    {
        memcpy(&size, stack->queue, sizeof(unsigned char));
        memcpy(elem, stack->queue+sizeof(unsigned char), size);
        stack->queue += 2*sizeof(unsigned char) + size;
    }
    else
    {
        size = stack->size;
        memcpy(elem, stack->queue, size);
        stack->queue += size;
    }
    return size;
}

int stack_queue_get_e(void* elem, struct stack_state* stack)
{
    if(stack->queue == stack->ptr)
        return -1;
    else
        return stack_queue_get(elem, stack);
}

struct stack_elem stack_queue_top_p(struct stack_state* stack)
{
    struct stack_elem elem;
    unsigned char size;
    if(stack->queue == stack->ptr)
        THROW(EXCEPTION_QUEUE_EMPTY);
    if(stack->mode & STACK_MODE_EXT)
    {
        memcpy(&size, stack->queue, sizeof(unsigned char));
        elem.size = (int)size;
        elem.ptr = stack->queue+sizeof(unsigned char);
    }
    else
    {
        elem.size = (int)stack->size;
        elem.ptr = stack->queue;
    }
    return elem;
}

void stack_queue_reset(struct stack_state* stack)
{
    stack->queue = stack->stack;
}

void stack_queue_next(struct stack_state* stack)
{
    unsigned char size;
    if(stack->queue == stack->ptr)
        THROW(EXCEPTION_QUEUE_EMPTY);
    if(stack->mode & STACK_MODE_EXT)
    {
        memcpy(&size, stack->queue, sizeof(unsigned char));
        stack->queue += 2*sizeof(unsigned char) + size;
    }
    else
        stack->queue += stack->size;
}

int stack_queue_empty(struct stack_state* stack)
{
    return stack->queue == stack->ptr;
}

void stack_digup(int deep, int count, struct stack_state* stack)
{
    int size1 = 0, max1 = 100;
    int size2 = 0, max2 = 100;
    char* tmp1 = malloc(max1*sizeof(char));
    char* tmp2 = malloc(max2*sizeof(char));
    if(!tmp1 || !tmp2)
        THROW(EXCEPTION_NO_MEMORY);
    
    int *max, *size, x, phase = 0;
    char* tmp, *ptr = stack->ptr;
    unsigned char y;
    while(1)
    {
        if(phase == 0)
        {
            x = deep;
            max = &max1;
            size = &size1;
            tmp = tmp1;
            phase = 1;
        }
        else
        {
            x = count;
            max = &max2;
            size = &size2;
            tmp = tmp2;
            phase = 0;
        }
        while(x-- > 0)
        {
            if(stack->mode & STACK_MODE_EXT)
            {
                ptr -= sizeof(unsigned char);
                memcpy(&y, ptr, sizeof(unsigned char));
                ptr -= y;
                if(*size+sizeof(unsigned char)+y >= *max)
                    if(!(tmp = realloc(tmp, *max *= 2)))
                        THROW(EXCEPTION_NO_MEMORY);
                memcpy(tmp+*size, ptr, y);
                *size += y;
                memcpy(tmp+*size, &y, sizeof(unsigned char));
                *size += sizeof(unsigned char);
                if(stack->mode & STACK_MODE_QUEUE)
                    ptr -= sizeof(unsigned char);
            }
            else
            {
                ptr -= stack->size;
                if(*size+stack->size >= *max)
                    if(!(tmp = realloc(tmp, *max *= 2)))
                        THROW(EXCEPTION_NO_MEMORY);
                memcpy(tmp+*size, ptr, stack->size);
                *size += stack->size;
            }
        }
        if(phase == 0)
            break;
    }
    while(1)
    {
        if(phase == 0)
        {
            x = deep;
            max = &max1;
            size = &size1;
            tmp = tmp1;
            phase = 1;
        }
        else
        {
            x = count;
            max = &max2;
            size = &size2;
            tmp = tmp2;
            phase = 0;
        }
        while(x-- > 0)
        {
            if(stack->mode & STACK_MODE_EXT)
            {
                *size -= sizeof(unsigned char);
                memcpy(&y, tmp+*size, sizeof(unsigned char));
                if(stack->mode & STACK_MODE_QUEUE)
                {
                    memcpy(ptr, &y, sizeof(unsigned char));
                    ptr += sizeof(unsigned char);
                }
                *size -= y;
                memcpy(ptr, tmp+*size, y);
                ptr += y;
                memcpy(ptr, &y, sizeof(unsigned char));
                ptr += sizeof(unsigned char);
            }
            else
            {
                *size -= stack->size;
                memcpy(ptr, tmp+*size, stack->size);
                ptr += stack->size;
            }
        }
        if(phase == 0)
            break;
    }
    free(tmp1);
    free(tmp2);
}
