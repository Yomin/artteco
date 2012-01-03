/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "list.h"

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

struct list_elem* iterate(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* current = list->first;
    while(current)
    {
        if(f(current->elem, param))
        {
            return current;
        }
        current = current->next;
    }
    return 0;
}

int map(void* elem, void* param)
{
    mapFunc* f = (mapFunc*) param;
    f(elem);
    return 0;
}

int fold(void* elem, void* param)
{
    struct fold_state* state = (struct fold_state*) param;
    state->f(elem, state->akk);
    return 0;
}

// EXTERNAL

struct list_state* list_init(int elemsize, struct list_state* list)
{
    list->elemsize = elemsize;
    list->first = 0;
    list->last = 0;
    list->current = 0;
    return list;
}

void* list_add(void* elem, struct list_state* list)
{
    struct list_elem* lelem = (struct list_elem*) malloc(sizeof(struct list_elem));
    lelem->elem = malloc(list->elemsize);
    lelem->next = 0;
    if(elem) memcpy(lelem->elem, elem, list->elemsize);
    else memset(lelem->elem, 0, list->elemsize);
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
    return lelem->elem;
}

void* list_add_s(struct list_state* list)
{
    return list_add(0, list);
}

int list_remove(matchFunc* f, void* param, struct list_state* list)
{
    struct list_elem* found = iterate(f, param, list);
    if(found)
    {
        if(!found->prev)
        {
            list->first = found->next;
        }
        else
        {
            found->prev->next = found->next;
        }
        if(!found->next)
        {
            list->last = found->prev;
        }
        else
        {
            found->next->prev = found->prev;
        }
        free(found->elem);
        free(found);
        return LIST_ELEM_FOUND;
    }
    else
    {
        return LIST_ELEM_NOTFOUND;
    }
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
    struct list_elem* found = iterate(f, param, list);
    if(found)
    {
        return found->elem;
    }
    else
    {
        return 0;
    }
}

void* list_get(int nth, struct list_state* list)
{
    list->current = list->first;
    while(list->current && nth-- > 0) list->current = list->current->next;
    if(list->current)
    {
        return list->current->elem;
    }
    else
    {
        return 0;
    }
}

void* list_current(struct list_state* list)
{
    if(!list->current) return 0;
    return list->current->elem;
}

void* list_next(struct list_state* list)
{
    if(!list->current || !list->current->next) return 0;
    list->current = list->current->next;
    return list->current->elem;
}

void* list_next_s(struct list_state* list)
{
    if(!list->current || !list->current->next) return 0;
    return list->current->next->elem;
}

void* list_prev(struct list_state* list)
{
    if(!list->current || !list->current->prev) return 0;
    list->current = list->current->prev;
    return list->current->elem;
}

void* list_prev_s(struct list_state* list)
{
    if(!list->current || !list->current->prev) return 0;
    return list->current->prev->elem;
}

struct list_state* list_map(mapFunc* f, struct list_state* list)
{
    iterate(map, f, list);
    return list;
}

struct list_state* list_fold(foldFunc* f, void* akk, struct list_state* list)
{
    struct fold_state state;
    state.f = f;
    state.akk = akk;
    iterate(fold, &state, list);
    return list;
}
