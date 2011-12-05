/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include <stdlib.h>

#include "screen.h"
#include "parse.h"

int main(int argc, char *argv[]) {
    
    screen_init();
    int ret = parse_start();
    screen_finish();
    
    return ret;
}
