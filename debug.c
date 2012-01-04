/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
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
    if(!file) file = dbgfile;
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
