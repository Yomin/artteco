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

#ifndef __ART_TECO_RUBOUT__
#define __ART_TECO_RUBOUT__

#include "debug.h"

typedef void rubout_func();

void  rubout_init();
void  rubout_finish();
DEBUGIZE_1(void,  rubout_register_s, rubout_func* f);
DEBUGIZE_3(void,  rubout_register, rubout_func* f, void* data, int size);
DEBUGIZE_2(void*, rubout_save, void* data, int size);
void  rubout_load(void* data);
int   rubout_top_size();
void* rubout_top_ptr();
void  rubout_start();
void  rubout_end();
void  rubout_break();
void  rubout_clear();
void  rubout();
void  rubout_ptr_register(rubout_func* f);
void  rubout_switch_top();

#ifndef NDEBUG
    #define rubout_register_s(f) rubout_register_s_dbg(f, STR(f), __func__)
    #define rubout_register(f, data, size) rubout_register_dbg(f, data, size, STR(f), STR(data), STR(size), __func__)
    #define rubout_save(data, size) rubout_save_dbg(data, size, STR(data), STR(size), __func__)
#endif

#endif
