/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
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

void screen_update_size();
void screen_clear();
void screen_refresh();

int screen_get_lines();
int screen_get_columns();

void screen_set_status(const char* status);
void screen_set_msg(const char* msg);
void screen_set_prompt(const char* prompt);
void screen_reset_prompt();

void screen_set_line(int num, const char* line);
void screen_set_line_o(int num, int offset, const char* line);
void screen_set_cursor(int y, int x);
void screen_move_cursor(int dir);

void screen_input_prompt_r(char c, int type);
void screen_input_prompt(char c, int type);
void screen_input_text_r(char c);
void screen_input_text(char c);
void screen_delete_prompt_r();
void screen_delete_prompt();
void screen_delete_text_r();
void screen_delete_text();

void screen_scroll(int n);

#endif
