/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_LIST__
#define __ART_TECO_LIST__

typedef int  matchFunc(void* elem, void* param);
typedef void mapFunc(void* elem);
typedef void foldFunc(void* elem, void* akk);

struct list_elem
{
    struct list_elem *next, *prev;
    void* elem;
};

struct list_state
{
    struct list_elem *first, *last, *current;
    int elemsize;
};

void list_init(int elemsize, struct list_state* list);
void list_clear(struct list_state* list);
void list_clear_f(mapFunc* f, struct list_state* list);

void* list_add_s(struct list_state* list);
void* list_add(void* elem, struct list_state* list);
void* list_add_sc(struct list_state* list);
void* list_add_c(void* elem, struct list_state* list);
int   list_remove(matchFunc* f, void* param, struct list_state* list);
int   list_remove_current(struct list_state* list);

void* list_find(matchFunc* f, void* param, struct list_state* list);
void* list_find_c(matchFunc* f, void* param, struct list_state* list);
void* list_get(int nth, struct list_state* list);
void* list_get_c(int nth, struct list_state* list);

void* list_current(struct list_state* list);
void* list_next(struct list_state* list);
void* list_next_s(struct list_state* list);
void* list_prev(struct list_state* list);
void* list_prev_s(struct list_state* list);

void list_map(mapFunc* f, struct list_state* list);
void list_fold(foldFunc* f, void* akk, struct list_state* list);

#endif
