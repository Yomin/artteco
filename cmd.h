/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_CMD__
#define __ART_TECO_CMD__

#define CMD_RET_SUCCESS 0
#define CMD_RET_FAILURE 1
#define CMD_RET_FINISH  2
#define CMD_RET_EXIT    3

#define CMD_MASK_RET    3 // choose bits containing return status
#define CMD_MASK_VALUE  4 // set/choose bit whether return value existent

struct cmd_ret
{
    int ret, value;
};

typedef struct cmd_ret cmd_init(int given, int param);  // cmd initialize function
typedef struct cmd_ret cmd_exec(char* str);             // cmd execute function
typedef int cmd_after();                                // after prompt finish function
typedef void cmd_rubout();                              // cmd rubout function
typedef void cmd_ecf(char c);                           // every char function

void cmd_start();
cmd_init* cmd_lookup(char c);

void cmd_reset_table();

#endif
