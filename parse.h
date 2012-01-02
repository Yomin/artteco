/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_PARSE__
#define __ART_TECO_PARSE__

#include "cmd.h"

void parse_init();
void parse_finish();
int  parse_input(char c);

void parse_register_func(cmd_func* f);
void parse_register_after(cmd_after* f);
void parse_register_ecf(cmd_ecf* f);
void parse_register_data(void* data, int size);

int parse_check_func();
int parse_check_after();
int parse_check_data();

cmd_func*  parse_get_func();
cmd_after* parse_get_after();
char*      parse_get_param();
void       parse_get_data(void* dst);

void parse_toggle_sign();

#endif
