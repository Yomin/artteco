/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "list.h"

#include <string.h>
#include <stdlib.h>

struct list_state* list_init(int elemsize, struct list_state* list)
{
    list->elemsize = elemsize;
    list->first = 0;
    list->last = 0;
    return list;
}

void* list_add(void* elem, struct list_state* list)
{
    struct list_elem* lelem = (struct list_elem*) malloc(sizeof(struct list_elem));
    lelem->elem = malloc(list->elemsize);
    lelem->next = 0;
    if(elem) memcpy(lelem->elem, elem, list->elemsize);
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

void list_clear_f(everyFunc* f, struct list_state* list)
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
