/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "debug.h"

#include <stdio.h>
#include <stdarg.h>

FILE* file;

void debug_start()
{
    file = fopen("log", "w");
}

void debug_stop()
{
    fclose(file);
}

void debug_log(const char* func, const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    fputs(func, file);
    fputs(": ", file);
    vfprintf(file, format, arglist);
    fputs("\n", file);
    va_end(arglist);
    #ifdef FLUSH
        fflush(file);
    #endif
}

void debug_flush()
{
    fflush(file);
}
