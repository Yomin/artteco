/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "debug.h"

#include <stdio.h>

FILE* file;

void debug_start()
{
    file = fopen("log", "w");
}

void debug_stop()
{
    fclose(file);
}

void debug_log(const char* func, const char* msg)
{
    fputs(func, file);
    fputs(": ", file);
    fputs(msg, file);
    fputc('\n', file);
}

void debug_logf(const char* func, const char* format, int var)
{
    fputs(func, file);
    fputs(": ", file);
    fprintf(file, format, var);
    fputc('\n', file);
}
