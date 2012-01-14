/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "list.h"
#include "exception.h"

#include <string.h>
#include <stdlib.h>

// FORWARDS

// DEFINES

struct fold_state
{
    foldFunc* f;
    void* akk;
};

// VARIABLES

// INTERNAL

struct list_elem* list_intern_iterate(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* current = list->first;
    while(current)
    {
        if(f(current->elem, param))
            return current;
        current = current->next;
    }
    return 0;
}

int list_intern_map(void* elem, void* param)
{
    mapFunc* f = (mapFunc*) param;
    f(elem);
    return 0;
}

int list_intern_fold(void* elem, void* param)
{
    struct fold_state* state = (struct fold_state*) param;
    state->f(elem, state->akk);
    return 0;
}

struct list_elem* list_intern_add(void* elem, struct list_state* list)
{
    struct list_elem* lelem = (struct list_elem*) malloc(sizeof(struct list_elem));
    if(!lelem)
        THROW(EXCEPTION_NO_MEMORY);
    lelem->elem = malloc(list->elemsize);
    if(!lelem->elem)
        THROW(EXCEPTION_NO_MEMORY);
    lelem->next = 0;
    if(elem)
        memcpy(lelem->elem, elem, list->elemsize);
    else
        memset(lelem->elem, 0, list->elemsize);
    if(!list->first)
    {
        list->first = lelem;
        list->last = lelem;
        lelem->prev = 0;
    }
    else
    {
        list->last->next = lelem;
        lelem->prev = list->last;
        list->last = lelem;
    }
    return lelem;
}

void list_intern_remove(struct list_elem* elem, struct list_state* list)
{
    if(!elem->prev)
        list->first = elem->next;
    else
        elem->prev->next = elem->next;
    if(!elem->next)
        list->last = elem->prev;
    else
        elem->next->prev = elem->prev;
    free(elem->elem);
    free(elem);
}

struct list_elem* list_intern_get(int nth, struct list_state* list)
{
    struct list_elem* elem = list->first;
    while(elem && --nth >= 0)
        elem = list->current->next;
    return elem;
}

// EXTERNAL

void list_init(int elemsize, struct list_state* list)
{
    list->elemsize = elemsize;
    list->first = 0;
    list->last = 0;
    list->current = 0;
}

void* list_add(void* elem, struct list_state* list)
{
    return list_intern_add(elem, list)->elem;
}

void* list_add_s(struct list_state* list)
{
    return list_intern_add(0, list)->elem;
}

void* list_add_c(void* elem, struct list_state* list)
{
    list->current = list_intern_add(elem, list);
    return list->current->elem;
}

void* list_add_sc(struct list_state* list)
{
    list->current = list_intern_add(0, list);
    return list->current->elem;
}

int list_remove(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* elem = list_intern_iterate(f, param, list);
    if(!elem)
        return -1;
    list_intern_remove(elem, list);
    return 0;
}

int list_remove_current(struct list_state* list)
{
    if(!list->current)
        return -1;
    list_intern_remove(list->current, list);
    return 0;
}

void list_clear_f(mapFunc* f, struct list_state* list)
{
    struct list_elem* current;
    struct list_elem* next = list->first;
    while(next)
    {
        current = next;
        next = current->next;
        if(f) f(current->elem);
        free(current->elem);
        free(current);
    }
    list->first = 0;
    list->last = 0;
}

void list_clear(struct list_state* list)
{
    list_clear_f(0, list);
}

void* list_find(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* found = list_intern_iterate(f, param, list);
    if(!found)
        return 0;
    return found->elem;
}

void* list_find_c(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* found = list_intern_iterate(f, param, list);
    if(!found)
        return 0;
    list->current = found;
    return found->elem;
}

void* list_get(int nth, struct list_state* list)
{
    struct list_elem* elem = list_intern_get(nth, list);
    if(!elem)
        return 0;
    return elem->elem;
}

void* list_get_c(int nth, struct list_state* list)
{
    struct list_elem* elem = list_intern_get(nth, list);
    if(!elem)
        return 0;
    list->current = elem;
    return elem->elem;
}

void* list_current(struct list_state* list)
{
    if(!list->current)
        return 0;
    return list->current->elem;
}

void* list_last(struct list_state* list)
{
    if(!list->last)
        return 0;
    return list->last->elem;
}

void* list_next(struct list_state* list)
{
    if(!list->current || !list->current->next)
        return 0;
    list->current = list->current->next;
    return list->current->elem;
}

void* list_next_s(struct list_state* list)
{
    if(!list->current || !list->current->next)
        return 0;
    return list->current->next->elem;
}

void* list_prev(struct list_state* list)
{
    if(!list->current || !list->current->prev)
        return 0;
    list->current = list->current->prev;
    return list->current->elem;
}

void* list_prev_s(struct list_state* list)
{
    if(!list->current || !list->current->prev)
        return 0;
    return list->current->prev->elem;
}

void list_map(mapFunc* f, struct list_state* list)
{
    list_intern_iterate(list_intern_map, f, list);
}

void list_fold(foldFunc* f, void* akk, struct list_state* list)
{
    struct fold_state state;
    state.f = f;
    state.akk = akk;
    list_intern_iterate(list_intern_fold, &state, list);
}
