/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_STACK__
#define __ART_TECO_STACK__

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
    int mode;
    int size;
    stack_func* func;
};

struct stack_elem
{
    char* ptr;
    int size;
};

struct stack_state* stack_init_s(struct stack_state* state);
struct stack_state* stack_init(int mode, int size, struct stack_state* state);
void* stack_push_s(void* elem, struct stack_state* stack);
void* stack_push_p(void* elemptr, int size, struct stack_state* stack);
void* stack_push_sp(void* elemptr, struct stack_state* stack);
void* stack_push(void* elem, int size, struct stack_state* stack);
struct stack_state* stack_pop_s(struct stack_state* stack);
int stack_pop(void* dst, struct stack_state* stack);
int stack_top(void* dst, struct stack_state* stack);
struct stack_elem stack_top_p(struct stack_state* stack);
void* stack_get_base(struct stack_state* stack);
void* stack_get_ptr(struct stack_state* stack);
struct stack_state* stack_set_base(void* ptr, struct stack_state* stack);
int stack_get_size(struct stack_state* stack);
int stack_empty(struct stack_state* stack);
int stack_empty_b(struct stack_state* stack);
struct stack_state* stack_clear(struct stack_state* stack);
struct stack_state* stack_set(void* src, int size, struct stack_state* stack);
struct stack_state* stack_set_size(int size, struct stack_state* stack);
struct stack_state* stack_set_func(stack_func* f, struct stack_state* stack);
int stack_exec(struct stack_state* stack);


#endif
