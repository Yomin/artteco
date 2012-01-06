/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_STACK__
#define __ART_TECO_STACK__

#include "debug.h"

#ifndef NDEBUG
    #include <stdio.h>
#endif

#define STACK_SIZE 1024

#define STACK_MODE_SIMPLE 0
#define STACK_MODE_EXT    1

#define STACK_EXEC_SUCCESS 0
#define STACK_EXEC_FAILURE 1

typedef int stack_func(void* elem, int size);

struct stack_state
{
    char stack[STACK_SIZE+1];
    char *base, *ptr;
    char mode;
    signed char size;
    stack_func* func;
    #ifndef NDEBUG
        FILE* file;
        char type[80];
    #endif
};

struct stack_elem
{
    char* ptr;
    int size;
};

struct stack_state* stack_init_s(const char* name, struct stack_state* state);
struct stack_state* stack_clear(struct stack_state* stack);
void                stack_finish(struct stack_state* stack);

#ifdef NDEBUG

    struct stack_state* stack_init(int mode, int size, const char* name, struct stack_state* state);

    void* stack_push(void* elem, int size, struct stack_state* stack);
    void* stack_push_s(void* elem, struct stack_state* stack);
    void* stack_push_p(void* elemptr, int size, struct stack_state* stack);
    void* stack_push_sp(void* elemptr, struct stack_state* stack);

#else
    
    #define stack_init(mode, size, name, state) stack_init_dbg(mode, size, name, state, #mode, #size, __func__)
    
    #define stack_push(elem, size, stack) stack_push_dbg(elem, size, stack, #elem, #size, __func__)
    #define stack_push_s(elem, stack) stack_push_s_dbg(elem, stack, #elem, __func__)
    #define stack_push_p(elemptr, size, stack) stack_push_p_dbg(elemptr, size, stack, #elemptr, #size, __func__)
    #define stack_push_sp(elemptr, stack) stack_push_sp_dbg(elemptr, stack, #elemptr, __func__)
    
    struct stack_state* stack_init_dbg(int mode, int size, const char* name, struct stack_state* state, const char* dbg_mode, const char* dbg_size, const char* dbg_func);
    
    void* stack_push_dbg(void* elem, int size, struct stack_state* stack, const char* dbg_elem, const char* dbg_size, const char* dbg_func);
    void* stack_push_s_dbg(void* elem, struct stack_state* stack, const char* dbg_elem, const char* dbg_func);
    void* stack_push_p_dbg(void* elemptr, int size, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_size, const char* dbg_func);
    void* stack_push_sp_dbg(void* elemptr, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_func);

#endif

int stack_pop(void* dst, struct stack_state* stack);
int stack_pop_s(struct stack_state* stack);

int               stack_top(void* dst, struct stack_state* stack);
struct stack_elem stack_top_p(struct stack_state* stack);

int stack_empty(struct stack_state* stack);
int stack_empty_b(struct stack_state* stack);

void* stack_get_base(struct stack_state* stack);
void* stack_get_ptr(struct stack_state* stack);
int   stack_get_size(struct stack_state* stack);

struct stack_state* stack_set(void* src, int size, struct stack_state* stack);
struct stack_state* stack_set_base(void* ptr, struct stack_state* stack);
struct stack_state* stack_set_size(int size, struct stack_state* stack);
struct stack_state* stack_set_func(stack_func* f, struct stack_state* stack);

int stack_exec(struct stack_state* stack);

#endif
