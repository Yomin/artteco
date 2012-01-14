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
#include "exception.h"

#include <ncurses.h>

int main(int argc, char *argv[])
{
    DEBUG_START();
    screen_init();
    
    screen_logo();
    char c = getch();
    screen_clear();
    screen_refresh();
    
    exception_init();
    rubout_init();
    buffer_mgr_init();
    cmd_init();
    parse_init();
    
    int ret;
    
    TRY
        ret = parse_input(c);
        while(ret < 0) ret = parse_input(getch());
    YRT
    CATCHALL
        screen_set_msg("EXCEPTION: '%s', press ex to exit",
            exception_str(exception_get()));
        while(getch() != 'e');
        screen_input_prompt('e', SCREEN_INPUT_TYPE_CMD);
        while(getch() != 'x');
        screen_input_prompt('x', SCREEN_INPUT_TYPE_CMD);
        ret = -1;
    HCTAC
    
    parse_finish();
    cmd_finish();
    buffer_mgr_finish();
    rubout_finish();
    exception_finish();
    screen_finish();
    DEBUG_STOP();
    
    return ret;
}
