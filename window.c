/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\
*/

#include "window.h"
#include <string.h>

struct window* window_init(struct window* win)
{
    memset(win, 0, sizeof(struct window));
    buffer_init(&(win->buffer));
    return win;
}

int window_move_cursor(int length, struct window* win)
{
    if(length < 0)
    {
        while(length > win->pos_cursor)
        {
            if(window_scroll(-1, win) == WINDOW_RET_FAILURE) return WINDOW_RET_FAILURE;
        }
        win->pos_cursor -= length;
    }
    else
    {
        while(length > win->pos_cursor_max - win->pos_cursor)
        {
            if(window_scroll(1, win) == WINDOW_RET_FAILURE) return WINDOW_RET_FAILURE;
        }
        win->pos_cursor += length;
    }
    return WINDOW_RET_SUCCESS;
}

int window_insert_str(char* str, struct window* win)
{
    if(win->type != WINDOW_TYPE_INS)
    {
        win->insert[win->pos_ins].pos = win->pos_cursor;
        win->type = WINDOW_TYPE_INS;
    }
    
    struct ins_buffer* ins = &win->insert[win->pos_ins];
    int size = strlen(str);
    
    if(ins->length >= WINDOW_INSERTBUF_SIZE-size)
    {
        int len = WINDOW_INSERTBUF_SIZE - ins->length;
        strncpy(ins->buffer+ins->length, str, len);
        ins->length += len;
        size -= len;
        if(win->pos_ins < WINDOW_BUFFER_SIZE-1)
        {
            win->pos_ins++;
            ins = &win->insert[win->pos_ins];
        }
        else
        {
            window_flush(win);
        }
    }

    strcpy(ins->buffer+ins->length, str);
    ins->length += size;
    win->pos_cursor++;
    
    return WINDOW_RET_SUCCESS;
}

int window_insert_char(char c, struct window* win)
{
    char str[2];
    str[0] = c;
    str[1] = 0;
    return window_insert_str((char*)&str, win);
}

int window_delete_str(int length, struct window* win)
{
    while(length > win->pos_cursor)
    {
        if(window_scroll(-1, win) == WINDOW_RET_FAILURE) return WINDOW_RET_FAILURE;
    }
    
    if(win->type != WINDOW_TYPE_DEL)
    {
        if(win->type == WINDOW_TYPE_INS)
        {
            int* inslen = &win->insert[win->pos_ins].length;
            if(length <= *inslen)
            {
                *inslen -= length;
                return WINDOW_RET_SUCCESS;
            }
            else
            {
                length -= *inslen;
                *inslen = 0;
                window_flush(win);
            }
        }
        win->delete[win->pos_del].pos = win->pos_cursor;
        win->type = WINDOW_TYPE_DEL;
    }
    win->delete[win->pos_del].length += length;
    
    return WINDOW_RET_SUCCESS;
}

int window_delete_char(struct window* win)
{
    return window_delete_str(1, win);
}

void window_flush(struct window* win)
{
    
}

int window_scroll(int length, struct window* win)
{
    
}
