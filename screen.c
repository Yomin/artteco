/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "screen.h"

#include <ncurses.h>
#include <string.h>
#include <signal.h>

// DEFINES

#define A_RED_BLACK     COLOR_PAIR(1) | A_BOLD
#define A_WHITE_RED     COLOR_PAIR(2) | A_BOLD
#define A_WHITE_BLACK   COLOR_PAIR(3) | A_BOLD
#define A_BLACK_GREEN   COLOR_PAIR(4) | A_BOLD
#define A_YELLOW_BLACK  COLOR_PAIR(5) | A_BOLD
#define A_GREY_BLACK    COLOR_PAIR(6) | A_BOLD

#define MOVE_PROMPT_FORWARD()   getyx(stdscr, tmp_y, tmp_x); move(tmp_y, tmp_x+1)
#define MOVE_PROMPT_BACKWARD()  getyx(stdscr, tmp_y, tmp_x); move(tmp_y, tmp_x-1)
#define MOVE_TEXT_FORWARD()     getyx(win, tmp_y, tmp_x); wmove(win, tmp_y, tmp_x+1)
#define MOVE_TEXT_BACKWARD()    getyx(win, tmp_y, tmp_x); wmove(win, tmp_y, tmp_x-1)

#define STORE_POS(win)      getyx(win, tmp_y, tmp_x);
#define RESTORE_POS(win)    wmove(win, tmp_y, tmp_x);

// FORWARDS

void print(int y, int x, const char* line, chtype attr);
void draw_cursor();
void undraw_cursor();
void resizeHandler(int sig);

// VARIABLES

WINDOW* win;
int lines, columns;
int tmp_y, tmp_x;

// EXTERNAL

void screen_init()
{
    initscr();
    refresh();
    
    win = newwin(LINES-3, COLS, 0, 0);
    scrollok(win, true);
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_BLACK);
    
    getmaxyx(stdscr, lines, columns);
    
    screen_set_status(" ART TECO");
    screen_set_msg("");
    screen_reset_prompt();
    
    signal(SIGWINCH, resizeHandler);
    
    noecho();
    //~ raw();
}

void screen_finish()
{
    endwin();
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

void screen_set_status(const char* status)
{
    print(lines-3, 0, status, A_WHITE_RED);
}

void screen_set_msg(const char* msg)
{
    print(lines-2, 0, msg, A_YELLOW_BLACK);
}

void screen_set_prompt(const char* prompt)
{
    print(lines-1, 0, prompt, A_RED_BLACK);
}

void screen_reset_prompt()
{
    print(lines-1, 0, "*", A_RED_BLACK);
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
    wmove(win, y, x);
    draw_cursor();
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

void screen_input_text(char c)
{
    winsch(win, c);
    MOVE_TEXT_FORWARD();
}

void screen_input_text_r(char c)
{
    screen_input_text(c);
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

void screen_delete_text()
{
    MOVE_TEXT_BACKWARD();
    wdelch(win);
}

void screen_delete_text_r()
{
    screen_delete_text();
    wrefresh(win);
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

void print(int y, int x, const char* line, chtype attr)
{
    STORE_POS(stdscr);
    move(y, x);
    attron(attr);
    printw("%s", line);
    int count = columns - strlen(line);
    while(count--) addch(' ');
    attroff(attr);
    RESTORE_POS(stdscr);
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
