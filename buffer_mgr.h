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

#ifndef __ART_TECO_BUFFER_MGR__
#define __ART_TECO_BUFFER_MGR__

#include "buffer.h"

#define BUFFER_MGR_ERROR_ACCESS_FORBIDDEN -1
#define BUFFER_MGR_ERROR_NOT_FOUND        -2
#define BUFFER_MGR_ERROR_EXISTING         -3
#define BUFFER_MGR_ERROR_FILE_NOT_FOUND   -4
#define BUFFER_MGR_ERROR_FILE_NAME_SIZE   -5
#define BUFFER_MGR_ERROR_FILE_NO_SPACE    -6
#define BUFFER_MGR_ERROR_FILE_CANT_WRITE  -7
#define BUFFER_MGR_ERROR_FILE_SRC_LOST    -8
#define BUFFER_MGR_ERROR_FILE_NAME_NEEDED -9


void buffer_mgr_init();
void buffer_mgr_finish();
void buffer_mgr_flush();

int  buffer_mgr_add(const char* name);
int  buffer_mgr_add_intern(const char* name);
int  buffer_mgr_add_file(const char* name, const char* file);
void buffer_mgr_add_new();
int  buffer_mgr_delete(int number);
int  buffer_mgr_save(int number, const char* name);
int  buffer_mgr_save_current(const char* name);

struct buffer_state* buffer_mgr_current();
struct buffer_state* buffer_mgr_switch(int number);

#endif
