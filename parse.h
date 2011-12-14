/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_PARSE__
#define __ART_TECO_PARSE__

#include "stack.h"
#include "cmd.h"
#include "buffer.h"

void parse_init();
void parse_finish();
int parse_start();

struct stack_state* parse_get_stk_input();
struct stack_state* parse_get_stk_gen();
struct stack_state* parse_get_stk_func();
struct stack_state* parse_get_stk_rub();
struct stack_state* parse_get_stk_after();

void parse_set_ecf(cmd_ecf* ecf);
void parse_set_ecf_rub(cmd_rubout* ecf_rub);
cmd_ecf* parse_get_ecf();
cmd_rubout* parse_get_ecf_rub();
void parse_toggle_sign();

#endif
