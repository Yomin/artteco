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
#define STACK_MODE_QUEUE  2

typedef int stack_func(void* elem, int size);

struct stack_state
{
    char stack[STACK_SIZE+1];
    char *base, *ptr, *queue;
    char mode;
    unsigned char size;
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

void stack_init_s(const char* name, struct stack_state* state);
void stack_clear(struct stack_state* stack);
void stack_finish(struct stack_state* stack);

#ifdef NDEBUG

    void stack_init(int mode, int size, const char* name, struct stack_state* state);

    void* stack_push(void* elem, int size, struct stack_state* stack);
    void* stack_push_s(void* elem, struct stack_state* stack);
    void* stack_push_p(void* elemptr, int size, struct stack_state* stack);
    void* stack_push_sp(void* elemptr, struct stack_state* stack);

#else
    
    #define stack_init(mode, size, name, state) stack_init_dbg(mode, size, name, state, STR(mode), STR(size), __func__)
    
    #define stack_push(elem, size, stack) stack_push_dbg(elem, size, stack, STR(elem), STR(size), __func__)
    #define stack_push_s(elem, stack) stack_push_s_dbg(elem, stack, STR(elem), __func__)
    #define stack_push_p(elemptr, size, stack) stack_push_p_dbg(elemptr, size, stack, STR(elemptr), STR(size), __func__)
    #define stack_push_sp(elemptr, stack) stack_push_sp_dbg(elemptr, stack, STR(elemptr), __func__)
    
    void stack_init_dbg(int mode, int size, const char* name, struct stack_state* state, const char* dbg_mode, const char* dbg_size, const char* dbg_func);
    
    void* stack_push_dbg(void* elem, int size, struct stack_state* stack, const char* dbg_elem, const char* dbg_size, const char* dbg_func);
    void* stack_push_s_dbg(void* elem, struct stack_state* stack, const char* dbg_elem, const char* dbg_func);
    void* stack_push_p_dbg(void* elemptr, int size, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_size, const char* dbg_func);
    void* stack_push_sp_dbg(void* elemptr, struct stack_state* stack, const char* dbg_elemptr, const char* dbg_func);

#endif

int stack_pop(void* dst, struct stack_state* stack);
int stack_pop_s(struct stack_state* stack);
int stack_pop_e(void* dst, struct stack_state* stack);
int stack_pop_se(struct stack_state* stack);

int               stack_top(void* dst, struct stack_state* stack);
int               stack_top_e(void* dst, struct stack_state* stack);
struct stack_elem stack_top_p(struct stack_state* stack);

int stack_empty(struct stack_state* stack);
int stack_empty_b(struct stack_state* stack);

void* stack_get_base(struct stack_state* stack);
void* stack_get_ptr(struct stack_state* stack);
int   stack_get_size(struct stack_state* stack);

void stack_set(void* src, int size, struct stack_state* stack);
void stack_set_base(void* ptr, struct stack_state* stack);
void stack_set_size(int size, struct stack_state* stack);
void stack_set_func(stack_func* f, struct stack_state* stack);

int stack_exec(struct stack_state* stack);

int stack_used_bytes(struct stack_state* stack);

int  stack_queue_get(void* elem, struct stack_state* stack);
int  stack_queue_get_e(void* elem, struct stack_state* stack);
void stack_queue_reset(struct stack_state* stack);

#endif
