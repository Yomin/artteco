/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "help.h"

int isNumeric(char c)
{
    return c >= 48 && c <= 57;
}

int getDigits(int n)
{
    if(!n) return 1;
    int d;
    for(d=0;n>0;n/=10,d++);
    return d;
}
