/*    _________________________
 *   / _ | _  / /_  __\  _\  _ \
 *  / __ |   / __/ / -_\ \_\ \\ \
 * /_/ |_|_\_\__/_/\___/\___\____\ 
 *
 * Copyright (c) 2012 Martin Rödel aka Yomin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
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
