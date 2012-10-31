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

#ifndef __ART_TECO_LIST__
#define __ART_TECO_LIST__

#define LIST_MODE_NORMAL 0
#define LIST_MODE_NXTPRV 1
#define LIST_MODE_BASE   2

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
    int size;
    char mode;
};

void list_init_s(int elemsize, struct list_state* list);
void list_init(char mode, int elemsize, struct list_state* list);
void list_clear(struct list_state* list);
void list_clear_f(mapFunc* f, struct list_state* list);

void* list_add_s(struct list_state* list);
void* list_add(void* elem, struct list_state* list);
void* list_add_sc(struct list_state* list);
void* list_add_c(void* elem, struct list_state* list);
int   list_delete(int pos, struct list_state* list);
int   list_remove(matchFunc* f, void* param, struct list_state* list);
int   list_remove_current(struct list_state* list);
void* list_insert(int pos, void* elem, struct list_state* list);
void* list_insert_after(matchFunc* f, void* param, void* elem, struct list_state* list);
void* list_insert_before(matchFunc* f, void* param, void* elem, struct list_state* list);
void* list_insert_next(void* elem, struct list_state* list);
void* list_insert_next_c(void* elem, struct list_state* list);
void* list_insert_prev(void* elem, struct list_state* list);
void* list_insert_prev_c(void* elem, struct list_state* list);

void* list_find(matchFunc* f, void* param, struct list_state* list);
void* list_find_c(matchFunc* f, void* param, struct list_state* list);
void* list_get(int pos, struct list_state* list);
void* list_get_c(int pos, struct list_state* list);
void* list_get_next(void* elem, struct list_state* list);
void* list_get_prev(void* elem, struct list_state* list);
int   list_pos(void* elem, struct list_state* list);
int   list_pos_c(void* elem, struct list_state* list);

// not recommended
struct list_state* list_get_base_d(void* elem, int size);
void*              list_get_next_d(void* elem, int size);
void*              list_get_prev_d(void* elem, int size);

void* list_current(struct list_state* list);
void* list_last(struct list_state* list);
void* list_next(struct list_state* list);
void* list_next_s(struct list_state* list);
void* list_prev(struct list_state* list);
void* list_prev_s(struct list_state* list);

void list_map(mapFunc* f, struct list_state* list);
void list_fold(foldFunc* f, void* akk, struct list_state* list);

#endif
