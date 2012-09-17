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

#ifndef __ART_TECO_CMD__
#define __ART_TECO_CMD__

#define CMD_RET_SUCCESS 0
#define CMD_RET_FAILURE 1
#define CMD_RET_FINISH  2
#define CMD_RET_EXIT    3

#define CMD_MASK_RET    3 // choose bits containing return status
#define CMD_MASK_VALUE  4 // set/choose bit whether return value existent
#define CMD_MASK_MSG    8 // whether screen msg was set

struct cmd_ret
{
    int ret, value;
};

typedef struct cmd_ret cmd_main(int given, int param);  // cmd initialize function
typedef struct cmd_ret cmd_func(char* str);             // cmd execute function
typedef int            cmd_after();                     // after prompt finish function
typedef void           cmd_ecf(char c);                 // every char function

// Extern

void cmd_init();
void cmd_finish();

cmd_main* cmd_lookup(char c);

void cmd_reset_table();
void cmd_switch_table(int table);

#endif
