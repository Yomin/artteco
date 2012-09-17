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

#ifndef NDEBUG

#include "debug.h"

#include <stdarg.h>

// FORWARDS

// DEFINES

// VARIABLES

FILE* dbgfile;

// INTERNAL

void debug_log_v(const char* func, FILE* file, const char* format, va_list arglist)
{
    if(!file)
        file = dbgfile;
    fputs(func, file);
    fputs(": ", file);
    vfprintf(file, format, arglist);
    fputs("\n", file);
    #ifdef FLUSH
        fflush(file);
    #endif
}

// EXTERNAL

void debug_start()
{
    dbgfile = fopen("logs/log", "w");
}

void debug_stop()
{
    fclose(dbgfile);
}

void debug_log(const char* func, const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    debug_log_v(func, 0, format, arglist);
    va_end(arglist);
}

void debug_log_f(const char* func, FILE* file, const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    debug_log_v(func, file, format, arglist);
    va_end(arglist);
}

void debug_flush()
{
    fflush(dbgfile);
}

#endif
