/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "debug.h"
#include "screen.h"
#include "buffer_mgr.h"
#include "cmd.h"
#include "parse.h"

int main(int argc, char *argv[])
{
    DEBUG_START();
    screen_init();
    buffer_mgr_init(screen_get_lines());
    cmd_init();
    parse_init();
    int ret = parse_start();
    parse_finish();
    cmd_finish();
    buffer_mgr_finish();
    screen_finish();
    DEBUG_STOP();
    
    return ret;
}
