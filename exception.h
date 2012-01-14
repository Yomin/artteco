/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_EXCEPTION__
#define __ART_TECO_EXCEPTION__

#include <setjmp.h>

#define EXCEPTION_IO                1
#define EXCEPTION_NO_MEMORY         2
#define EXCEPTION_STACK_OVERFLOW    3
#define EXCEPTION_STACK_EMPTY       4
#define EXCEPTION_FUNCTION_MISSING  5
#define EXCEPTION_OBJECT_SIZE       6
#define EXCEPTION_BUFFER_OVERFLOW   7


#define TRY \
{ \
    int exception = 0; \
    jmp_buf* buf = exception_point_init(); \
    setjmp(*buf); \
    if(!exception_point_start()) \
    {

#define YRT \
    } \
    else \
    { \
        exception = 1; \
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
    if(!exception) \
        exception_done(); \
}

#define THROW(E) \
    exception_throw(E);


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
