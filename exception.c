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

#include "exception.h"
#include "stack.h"

#include <string.h>

// FORWARDS

// DEFINES

struct exception_buf
{
    char jmp;
    jmp_buf buf;
};

// VARIABLES

struct stack_state stk_exc;
int exception;

// INTERNAL

// EXTERNAL

void exception_init()
{
    stack_init(STACK_MODE_SIMPLE, sizeof(struct exception_buf), "exception", &stk_exc);
}

void exception_finish()
{
    stack_finish(&stk_exc);
}

jmp_buf* exception_point_init()
{
    struct exception_buf buf;
    buf.jmp = 0;
    struct exception_buf* sbuf = (struct exception_buf*)stack_push_s(&buf, &stk_exc);
    return &sbuf->buf;
}

int exception_point_start()
{
    struct stack_elem elem = stack_top_p(&stk_exc);
    char* jmp = &((struct exception_buf*)elem.ptr)->jmp;
    if(!*jmp)
    {
        *jmp = 1;
        return 0;
    }
    else
    {
        stack_pop_s(&stk_exc);
        return 1;
    }
}

void exception_throw(int exc)
{
    exception = exc;
    struct exception_buf buf;
    stack_top(&buf, &stk_exc);
    longjmp(buf.buf, 1);
}

void exception_chain()
{
    struct exception_buf buf;
    stack_top(&buf, &stk_exc);
    longjmp(buf.buf, 1);
}

void exception_done()
{
    stack_pop_s(&stk_exc);
}

void exception_set(int exc)
{
    exception = exc;
}

int  exception_get()
{
    return exception;
}

const char* exception_str(int exc)
{
    switch(exc)
    {
        case EXCEPTION_IO:               return "io error";
        case EXCEPTION_NO_MEMORY:        return "unsufficient memory";
        case EXCEPTION_NO_SPACE:         return "unsufficient file system space";
        case EXCEPTION_STACK_OVERFLOW:   return "stack overflow";
        case EXCEPTION_STACK_EMPTY:      return "stack empty";
        case EXCEPTION_FUNCTION_MISSING: return "function missing";
        case EXCEPTION_OBJECT_SIZE:      return "object exceeds allowed size";
        case EXCEPTION_BUFFER_OVERFLOW:  return "buffer overflow";
        case EXCEPTION_QUEUE_EMPTY:      return "queue empty";
        case EXCEPTION_WRONG_CONFIG:     return "wrong configuration";
        case EXCEPTION_FUNCTION_MISSUSE: return "function missuse";
        case EXCEPTION_UNKNOWN_RETURN:   return "return value unknown";
    }
    return "unknown exception";
}
