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

#ifndef __ART_TECO_EXCEPTION__
#define __ART_TECO_EXCEPTION__

#include <setjmp.h>

#define EXCEPTION_IO                1
#define EXCEPTION_NO_MEMORY         2
#define EXCEPTION_NO_SPACE          3
#define EXCEPTION_STACK_OVERFLOW    4
#define EXCEPTION_STACK_EMPTY       5
#define EXCEPTION_FUNCTION_MISSING  6
#define EXCEPTION_OBJECT_SIZE       7
#define EXCEPTION_BUFFER_OVERFLOW   8
#define EXCEPTION_QUEUE_EMPTY       9
#define EXCEPTION_WRONG_CONFIG      10
#define EXCEPTION_FUNCTION_MISSUSE  11
#define EXCEPTION_UNKNOWN_RETURN    12


#define TRY \
{ \
    jmp_buf* buf = exception_point_init(); \
    setjmp(*buf); \
    if(!exception_point_start()) \
    {

#define YRT \
    exception_done(); \
    } \
    else \
    { \
        int matched = 0; \
        switch(exception_get()) \
        { \
            case -1: \
            {

#define CATCH(E) \
                matched = 1; \
                break; \
            } \
            case E: \
            {

#define CATCHALL \
                matched = 1; \
                break; \
            } \
            default: \
            {

#define HCTAC \
                matched = 1; \
            } \
        } \
        if(!matched) \
            exception_chain(); \
    } \
}

#define THROW(E) \
    exception_throw(E)


void exception_init();
void exception_finish();

jmp_buf* exception_point_init();
int      exception_point_start();

void exception_throw(int exc);
void exception_chain();
void exception_done();

void exception_set(int exc);
int  exception_get();

const char* exception_str(int exc);

#endif
