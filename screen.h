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

#ifndef __ART_TECO_SCREEN__
#define __ART_TECO_SCREEN__

#define SCREEN_INPUT_TYPE_CMD   0
#define SCREEN_INPUT_TYPE_TXT   1
#define SCREEN_INPUT_TYPE_ESC   2

#define SCREEN_CURSOR_FORWARD   0
#define SCREEN_CURSOR_BACKWARD  1
#define SCREEN_CURSOR_UP        2
#define SCREEN_CURSOR_DOWN      3

void screen_init();
void screen_finish();
void screen_logo();

void screen_update_size();
void screen_clear();
void screen_refresh();

int screen_get_lines();
int screen_get_columns();
int screen_get_buffer_lines();
int screen_get_buffer_columns();

void screen_set_status(const char* status, ...);
void screen_set_msg(const char* msg, ...);
void screen_set_prompt(const char* prompt, ...);
void screen_reset_prompt();

void screen_set_line(int num, const char* line);
void screen_set_line_o(int num, int offset, const char* line);
void screen_set_cursor(int y, int x);
void screen_get_cursor(int* y, int* x);
void screen_move_cursor(int dir);
void screen_set_pos(int pos);
int  screen_get_pos();

void screen_input_prompt_r(char c, int type);
void screen_input_prompt(char c, int type);
void screen_input_text_cr(char c);
void screen_input_text_c(char c);
void screen_input_text_sr(const char* str);
void screen_input_text_s(const char* str);
void screen_delete_prompt_r();
void screen_delete_prompt();
char screen_delete_text_r();
char screen_delete_text();
char screen_get_text(int pos);

void screen_scroll(int n);

#endif
