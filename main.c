/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "debug.h"
#include "screen.h"
#include "buffer_mgr.h"
#include "cmd.h"
#include "rubout.h"
#include "parse.h"

#include <ncurses.h>

int main(int argc, char *argv[])
{
    DEBUG_START();
    rubout_init();
    screen_init();
    buffer_mgr_init();
    cmd_init();
    parse_init();
    
    int ret = -1;
    while(ret < 0) ret = parse_input(getch());
    
    parse_finish();
    cmd_finish();
    buffer_mgr_finish();
    screen_finish();
    rubout_finish();
    DEBUG_STOP();
    
    return ret;
}
