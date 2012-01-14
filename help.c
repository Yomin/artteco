/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#include "help.h"

int is_numeric(char c)
{
    return c >= 48 && c <= 57;
}

int get_digits(int n)
{
    if(!n) return 1;
    int d;
    for(d=0; n>0; n/=10, ++d);
    return d;
}

int power(int b, int e)
{
    int x=1;
    if(b!=0 || e!=0)
    {
        for(; e>0; --e)
            x *= b;
    }
    return x;
}

