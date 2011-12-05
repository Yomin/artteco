/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_WINDOW__
#define __ART_TECO_WINDOW__

#include "buffer.h"

#define WINDOW_INSERTBUF_SIZE   80
#define WINDOW_BUFFER_SIZE      10

#define WINDOW_TYPE_NO  0
#define WINDOW_TYPE_INS 1
#define WINDOW_TYPE_DEL 2

#define WINDOW_RET_SUCCESS  0
#define WINDOW_RET_FAILURE  1

struct ins_buffer
{
    int pos, length;
    char buffer[WINDOW_INSERTBUF_SIZE];
};

struct del_buffer
{
    int pos, length;
};

struct window
{
    struct buffer_state buffer;
    int pos_cursor, pos_screen, pos_cursor_max, pos_screen_max;
    int pos_ins, pos_del;
    char type;
    struct ins_buffer insert[WINDOW_BUFFER_SIZE];
    struct ins_buffer delete[WINDOW_BUFFER_SIZE];
};

struct window* window_init(struct window* win);
int window_move_cursor(int length, struct window* win);
int window_insert_char(char c, struct window* win);
int window_insert_str(char* str, struct window* win);
int window_delete_char(struct window* win);
int window_delete_str(int length, struct window* win);
void window_flush(struct window* win);
int window_scroll(int length, struct window* win);

#endif
