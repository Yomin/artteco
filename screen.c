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

#define NEWPOS(y, x)    getyx(stdscr, tmp_y, tmp_x); move(y, x)
#define OLDPOS()        move(tmp_y, tmp_x)
#define WNEWPOS(y, x)   getyx(stdscr, tmp_y, tmp_x); wmove(win, y, x)
#define WOLDPOS()       wmove(win, tmp_y, tmp_x)

// FORWARDS

void draw_status();
void draw_msg();
void draw_prompt();
void draw_textcursor();
void resizeHandler(int sig);

// VARIABLES

WINDOW* win;
int lines, columns;
char status_line[80];
char msg_line[80];
int textcursor_pos_x, textcursor_pos_y;
int tmp_x, tmp_y;
const char* tab_esc[32];

// EXTERNAL

void screen_init()
{
    initscr();
    refresh();
    
    win = stdscr;
    //~ win = newwin(LINES-3, COLS, 0, 0);
    //~ wrefresh(win);
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_BLACK);
    
    getmaxyx(stdscr, lines, columns);
    screen_set_status(" ART TECO");
    screen_set_prompt("*");
    textcursor_pos_x = 0;
    textcursor_pos_y = 0;
    draw_textcursor();
    move(lines-1, 1);
    signal(SIGWINCH, resizeHandler);
    noecho();
    
    tab_esc[ 0] = "NUL";
    tab_esc[ 1] = "SOH";
    tab_esc[ 2] = "STX";
    tab_esc[ 3] = "ETX";
    tab_esc[ 4] = "EOT";
    tab_esc[ 5] = "ENQ";
    tab_esc[ 6] = "ACK";
    tab_esc[ 7] = "BEL";
    tab_esc[ 8] = "BS";
    tab_esc[ 9] = "TAB";
    tab_esc[10] = "LF";
    tab_esc[11] = "VT";
    tab_esc[12] = "FF";
    tab_esc[13] = "CR";
    tab_esc[14] = "SO";
    tab_esc[15] = "SI";
    tab_esc[16] = "DLE";
    tab_esc[17] = "DC1";
    tab_esc[18] = "DC2";
    tab_esc[19] = "DC3";
    tab_esc[20] = "DC4";
    tab_esc[21] = "NAK";
    tab_esc[22] = "SYN";
    tab_esc[23] = "ETB";
    tab_esc[24] = "CAN";
    tab_esc[25] = "EM";
    tab_esc[26] = "SUB";
    tab_esc[27] = "ESC";
    tab_esc[28] = "FS";
    tab_esc[29] = "GS";
    tab_esc[30] = "RS";
    tab_esc[31] = "US";
    
}

void screen_finish()
{
    endwin();
}

void screen_update_size()
{
    endwin();
    initscr();
    getmaxyx(stdscr, lines, columns);
}

void screen_set_status(char* status)
{
    strncpy(status_line, status, 80);
    draw_status();
}

void screen_set_msg(char* msg)
{
    strncpy(msg_line, msg, 80);
    draw_msg();
}

void screen_set_prompt(char* prompt)
{
    draw_prompt();
    move(lines-1, 1);
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
    attron(att);
    if(c < 32) printw("<%s>", tab_esc[c]);
    else addch(c);
    attroff(att);
}

void screen_input_text(char c)
{
    WNEWPOS(textcursor_pos_y, textcursor_pos_x);
    winsch(win, c);
    waddch(win, c|A_WHITE_BLACK);
    wrefresh(win);
    getyx(stdscr, textcursor_pos_y, textcursor_pos_x);
    OLDPOS();
}

void screen_delete_prompt()
{
    int y, x;
    getyx(stdscr, y, x);
    x--;
    move(y, x);
    int c = inch()&A_CHARTEXT;
    if(c == '>')
    {
        while(c != '<')
        {
            delch();
            x--;
            move(y, x);
            c = inch()&A_CHARTEXT;
        }
    }
    delch();
}

void screen_delete_text()
{
    textcursor_pos_x--;
    WNEWPOS(textcursor_pos_y, textcursor_pos_x);
    wdelch(win);
    //~ int c = winch(win);
    //~ waddch(win, c|A_BLACK_GREEN);
    wrefresh(win);
    OLDPOS();
}

void screen_scroll(int n)
{
    scrollok(win, true);
    wscrl(win, n);
    wrefresh(win);  
}

// HANDLER

void resizeHandler(int sig)
{
    screen_update_size();
    draw_status();
}

// INTERNAL

void draw_status()
{
    NEWPOS(lines-3, 0);
    attron(A_WHITE_RED);
    printw("%s", status_line);
    int count = columns - strlen(status_line);
    while(count--) addch(' ');
    attroff(A_WHITE_RED);
    OLDPOS();
    refresh();
}

void draw_prompt()
{
    NEWPOS(lines-1, 0);
    attron(A_RED_BLACK);
    addch('*');
    int count = columns - strlen(msg_line);
    while(count--) addch(' ');
    attroff(A_RED_BLACK);
    OLDPOS();
    refresh();
}

void draw_textcursor()
{
    WNEWPOS(textcursor_pos_y, textcursor_pos_x);
    waddch(win, ' '|A_BLACK_GREEN);
    OLDPOS();
    wrefresh(win);
}

void draw_msg()
{
    NEWPOS(lines-2, 0);
    attron(A_YELLOW_BLACK);
    printw("%s", msg_line);
    int count = columns - strlen(msg_line);
    while(count--) addch(' ');
    attroff(A_YELLOW_BLACK);
    OLDPOS();
    refresh();
}
