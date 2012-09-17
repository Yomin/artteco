/*    _________________________
 *   / _ | _  / /_  __\  _\  _ \
 *  / __ |   / __/ / -_\ \_\ \\ \
 * /_/ |_|_\_\__/_/\___/\___\____\ 
 *
 * Copyright (c) 2012 Martin Rödel aka Yomin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
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

struct list_elem* list_intern_gen(void* elem, struct list_state* list)
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
    return lelem;
}

struct list_elem* list_intern_add(void* elem, struct list_state* list)
{
    struct list_elem* lelem = list_intern_gen(elem, list);
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
    ++list->size;
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
    --list->size;
}

struct list_elem* list_intern_get(int pos, struct list_state* list)
{
    if(pos >= list->size) return 0;
    if(pos >= list->size/2)
    {
        pos = list->size - pos -1;
        struct list_elem* elem = list->last;
        while(elem && --pos >= 0)
            elem = elem->prev;
        return elem;
    }
    else
    {
        struct list_elem* elem = list->first;
        while(elem && --pos >= 0)
            elem = elem->next;
        return elem;
    }
}

// EXTERNAL

void list_init(int elemsize, struct list_state* list)
{
    list->elemsize = elemsize;
    list->size = 0;
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

int list_delete(int pos, struct list_state* list)
{
    struct list_elem* elem = list_intern_get(pos, list);
    if(!elem)
        return -1;
    list_intern_remove(elem, list);
    return 0;
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

void* list_insert(int pos, void* elem, struct list_state* list)
{
    struct list_elem* felem = list_intern_get(pos, list);
    if(felem)
    {
        struct list_elem* lelem = list_intern_gen(elem, list);
        lelem->prev = felem->prev;
        if(felem->prev)
            felem->prev->next = lelem;
        lelem->next = felem;
        felem->prev = lelem;
        ++list->size;
        return lelem;
    }
    if(pos == list->size)
    {
        struct list_elem* lelem = list_intern_gen(elem, list);
        lelem->prev = list->last;
        list->last->next = lelem;
        lelem->next = 0;
        ++list->size;
        return lelem;
    }
    return 0;
}

void* list_insert_after(matchFunc* f, void* param, void* elem, struct list_state* list)
{
    struct list_elem* felem = list_intern_iterate(f, param, list);
    if(!felem)
        return 0;
    struct list_elem* lelem = list_intern_gen(elem, list);
    lelem->next = felem->next;
    if(felem->next)
        felem->next->prev = lelem;
    felem->next = lelem;
    lelem->prev = felem;
    return lelem;
}

void* list_insert_before(matchFunc* f, void* param, void* elem, struct list_state* list)
{
    struct list_elem* felem = list_intern_iterate(f, param, list);
    if(!felem)
        return 0;
    struct list_elem* lelem = list_intern_gen(elem, list);
    lelem->prev = felem->prev;
    if(felem->prev)
        felem->prev->next = lelem;
    felem->prev = lelem;
    lelem->next = felem;
    return lelem;
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

void* list_get(int pos, struct list_state* list)
{
    struct list_elem* elem = list_intern_get(pos, list);
    if(!elem)
        return 0;
    return elem->elem;
}

void* list_get_c(int pos, struct list_state* list)
{
    struct list_elem* elem = list_intern_get(pos, list);
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
