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

#include "screen.h"

#include <ncurses.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

// DEFINES

#define A_RED_BLACK     COLOR_PAIR(1) | A_BOLD
#define A_WHITE_RED     COLOR_PAIR(2) | A_BOLD
#define A_WHITE_BLACK   COLOR_PAIR(3) | A_BOLD
#define A_BLACK_GREEN   COLOR_PAIR(4) | A_BOLD
#define A_YELLOW_BLACK  COLOR_PAIR(5) | A_BOLD
#define A_GREY_BLACK    COLOR_PAIR(6) | A_BOLD

#define MOVE_PROMPT_FORWARD()   getyx(stdscr, tmp_y, tmp_x); move(tmp_y, tmp_x+1)
#define MOVE_PROMPT_BACKWARD()  getyx(stdscr, tmp_y, tmp_x); move(tmp_y, tmp_x-1)
#define MOVE_TEXT_FORWARD(pos)  getyx(win, tmp_y, tmp_x); wmove(win, tmp_y, tmp_x+pos)
#define MOVE_TEXT_BACKWARD(pos) getyx(win, tmp_y, tmp_x); wmove(win, tmp_y, tmp_x-pos)

#define STORE_POS(win)      getyx(win, tmp_y, tmp_x);
#define RESTORE_POS(win)    wmove(win, tmp_y, tmp_x);

#define STATUS_PREFIX " ART TECO "

// FORWARDS

void  print(int y, int x, chtype attr, const char* line, ...);
void vprint(int y, int x, chtype attr, const char* line, va_list arglist);
void draw_cursor();
void undraw_cursor();
void resizeHandler(int sig);

// VARIABLES

WINDOW* win;
int lines, columns;
int tmp_y, tmp_x;
char* tmpbuf;
int bufsize;

// EXTERNAL

void screen_init()
{
    initscr();
    refresh();
    
    win = newwin(LINES-3, COLS, 0, 0);
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_BLACK);
    
    getmaxyx(stdscr, lines, columns);
    tmpbuf = malloc(columns*sizeof(char));
    bufsize = columns;
    
    screen_reset_status();
    screen_set_msg("");
    screen_reset_prompt();
    
    signal(SIGWINCH, resizeHandler);
    
    noecho();
    //~ raw();
}

void screen_finish()
{
    if(tmpbuf)
        free(tmpbuf);
    endwin();
}

void screen_logo()
{
    char* line[7];
    line[0] = "      ________________________________________";
    line[1] = "     /   |          /  /        \\      \\      \\";
    line[2] = "    / __ |  ____   /  /___   ____\\   ___\\  ___ \\";
    line[3] = "   /     |        /      /  /  _  \\  \\   \\ \\  \\ \\";
    line[4] = "  /  __  |  __   /  ____/  /     __\\  \\___\\ \\  \\ \\";
    line[5] = " /  / |  |  \\ \\  \\     /  /\\       /\\      \\      \\";
    line[6] = "/__/  |__|___\\ \\__\\___/__/  \\_____/  \\______\\______\\";
    
    int i, starty = lines/4, startx = columns/2-strlen(line[6])/2;
    
    wattron(win, A_RED_BLACK);
    for(i=0; i<7; ++i)
        mvwprintw(win, starty+i, startx, line[i]);
    wattroff(win, A_RED_BLACK);
    
    wrefresh(win);
}

int screen_get_lines()
{
    return lines;
}

int screen_get_columns()
{
    return columns;
}

int screen_get_buffer_lines()
{
    return lines-3;
}

int screen_get_buffer_columns()
{
    return columns;
}

void screen_update_size()
{
    //~ endwin();
    //~ initscr();
    //~ getmaxyx(stdscr, lines, columns);
}

void screen_clear()
{
    wclear(win);
}

void screen_refresh()
{
    wrefresh(win);
}

void screen_set_status(const char* status, ...)
{
    va_list arglist;
    va_start(arglist, status);
    vprint(lines-3, strlen(STATUS_PREFIX), A_WHITE_RED, status, arglist);
    va_end(arglist);
}

void screen_set_msg(const char* msg, ...)
{
    va_list arglist;
    va_start(arglist, msg);
    vprint(lines-2, 0, A_YELLOW_BLACK, msg, arglist);
    va_end(arglist);
}

void screen_set_prompt(const char* prompt, ...)
{
    va_list arglist;
    va_start(arglist, prompt);
    vprint(lines-1, 0, A_RED_BLACK, prompt, arglist);
    va_end(arglist);
}

void screen_reset_status()
{
    print(lines-3, 0, A_WHITE_RED, STATUS_PREFIX);
}

void screen_reset_prompt()
{
    print(lines-1, 0, A_RED_BLACK, "*");
    move(lines-1, 1);
}

void screen_set_line_o(int num, int offset, const char* line)
{
    wmove(win, num, offset);
    wprintw(win, line);
}

void screen_set_line(int num, const char* line)
{
    screen_set_line_o(num, 0, line);
}

void screen_set_cursor(int y, int x)
{
    undraw_cursor();
    wmove(win, y, x);
    draw_cursor();
}

void screen_get_cursor(int* y, int* x)
{
    getyx(win, *y, *x);
}

void screen_set_pos(int pos)
{
    int y = pos/columns;
    int x = pos%columns;
    screen_set_cursor(y, x);
}

int screen_get_pos()
{
    int y, x;
    screen_get_cursor(&y, &x);
    return y*columns+x;
}

void screen_move_cursor(int dir)
{
    undraw_cursor();
    getyx(win, tmp_y, tmp_x);
    switch(dir)
    {
        case SCREEN_CURSOR_FORWARD:  wmove(win, tmp_y, tmp_x+1); break;
        case SCREEN_CURSOR_BACKWARD: wmove(win, tmp_y, tmp_x-1); break;
        case SCREEN_CURSOR_UP:       wmove(win, tmp_y-1, tmp_x); break;
        case SCREEN_CURSOR_DOWN:     wmove(win, tmp_y+1, tmp_x); break;
    }
    draw_cursor();
}

void screen_input_prompt(char c, int type)
{
    int att;
    switch(type)
    {
        case SCREEN_INPUT_TYPE_CMD: att = A_RED_BLACK; break;
        case SCREEN_INPUT_TYPE_TXT: att = A_WHITE_BLACK; break;
        case SCREEN_INPUT_TYPE_ESC: att = A_GREY_BLACK; break;
    }
    addch(c|att);
}

void screen_input_prompt_r(char c, int type)
{
    screen_input_prompt(c, type);
    refresh();
}

void screen_input_text_c(char c)
{
    winsch(win, c);
    MOVE_TEXT_FORWARD(1);
}

void screen_input_text_cr(char c)
{
    screen_input_text_c(c);
    wrefresh(win);
}

void screen_input_text_s(int len, int linesize, const char* str)
{
    int y, x;
    getyx(win, y, x);
    int rest = linesize-x;
    
    if(x+len+rest < columns)
    {
        winsstr(win, str);
        wmove(win, y, x+len);
    }
    else if(x+len+rest == columns)
    {
        winsstr(win, str);
        wmove(win, y+1, 0);
        winsertln(win);
        draw_cursor();
    }
    else if(x+len <= columns)
    {
        int wrap = rest - (columns-(x+len));
        if(wrap >= bufsize)
        {
            tmpbuf = realloc(tmpbuf, (wrap+1)*sizeof(char));
            bufsize = wrap+1;
        }
        winnstr(win, tmpbuf, wrap);
        tmpbuf[wrap] = 0;
        winsstr(win, str);
        wmove(win, y+1, 0);
        winsertln(win);
        waddstr(win, tmpbuf);
        draw_cursor();
    }
    else
    {
        if(rest >= bufsize)
        {
            tmpbuf = realloc(tmpbuf, (rest+1)*sizeof(char));
            bufsize = rest+1;
        }
        winnstr(win, tmpbuf, rest);
        tmpbuf[rest] = 0;
        waddstr(win, str);
        len -= columns-x;
        str += columns-x;
        while(len > columns)
        {
            waddstr(win, str);
            len -= columns;
            str += columns;
        }
        waddstr(win, str);
        waddstr(win, tmpbuf);
        draw_cursor();
    }
}

void screen_input_text_sr(int len, int linesize, const char* str)
{
    screen_input_text_s(len, linesize, str);
    wrefresh(win);
}

void screen_delete_prompt()
{
    MOVE_PROMPT_BACKWARD();
    delch();
}

void screen_delete_prompt_r()
{
    screen_delete_prompt();
    refresh();
}

char screen_delete_text()
{
    MOVE_TEXT_BACKWARD(1);
    char c = winch(win) & A_CHARTEXT;
    wdelch(win);
    return c;
}

char screen_delete_text_r()
{
    char c = screen_delete_text();
    wrefresh(win);
    return c;
}

char screen_get_text(int pos)
{
    MOVE_TEXT_BACKWARD(pos);
    char c = winch(win) & A_CHARTEXT;
    MOVE_TEXT_FORWARD(pos);
    return c;
}

void screen_scroll(int n)
{
    wscrl(win, n);
    wrefresh(win);  
}

// HANDLER

void resizeHandler(int sig)
{
    //~ screen_update_size();
    //~ draw_status();
}

// INTERNAL

void vprint(int y, int x, chtype attr, const char* line, va_list arglist)
{
    STORE_POS(stdscr);
    move(y, x);
    attron(attr);
    vw_printw(stdscr, line, arglist);
    int yy, xx;
    getyx(stdscr, yy, xx);
    int count = columns - xx;
    while(--count >= 0)
        addch(' ');
    attroff(attr);
    RESTORE_POS(stdscr);
}

void print(int y, int x, chtype attr, const char* line, ...)
{
    va_list arglist;
    va_start(arglist, line);
    vprint(y, x, attr, line, arglist);
    va_end(arglist);
}

void draw_cursor()
{
    STORE_POS(win);
    char c = winch(win) & A_CHARTEXT;
    waddch(win, c | A_BLACK_GREEN);
    RESTORE_POS(win);
}

void undraw_cursor()
{
    STORE_POS(win);
    char c = winch(win) & A_CHARTEXT;
    waddch(win, c);
    RESTORE_POS(win);
}
