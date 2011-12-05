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

void screen_init();
void screen_finish();

void screen_update_size();
void screen_set_status(char* status);
void screen_set_msg(char* msg);
void screen_set_prompt(char* prompt);

void screen_input_prompt(char c, int type);
void screen_input_text(char c);
void screen_delete_prompt();
void screen_delete_text();

void screen_scroll(int n);

#endif
