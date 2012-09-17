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

#ifndef __ART_TECO_DEBUG__
#define __ART_TECO_DEBUG__

#ifndef NDEBUG

#include <stdio.h>

#define DEBUG_LOG(f, ...)           debug_log(__func__, f, ##__VA_ARGS__)
#define DEBUG_LOG_F(file, f, ...)   debug_log_f(__func__, file, f, ##__VA_ARGS__)
#define DEBUG_FLUSH()               debug_flush(__func__)
#define DEBUG_START()               debug_start()
#define DEBUG_STOP()                debug_stop()

void debug_start();
void debug_stop();
void debug_log(const char* func, const char* format, ...);
void debug_log_f(const char* func, FILE* file, const char* format, ...);
void debug_flush();

#define DEBUG(release, debug) debug

#define DEBUGIZE_0(ret, func, ...) ret func(__VA_ARGS__, const char* dbg_func)
#define DEBUGIZE_1(ret, func, arg, ...) ret func##_dbg(arg, ##__VA_ARGS__, const char* dbg_arg, const char* dbg_func)
#define DEBUGIZE_2(ret, func, arg1, arg2, ...) ret func##_dbg(arg1, arg2, ##__VA_ARGS__, const char* dbg_arg1, const char* dbg_arg2, const char* dbg_func)
#define DEBUGIZE_3(ret, func, arg1, arg2, arg3, ...) ret func##_dbg(arg1, arg2, arg3, ##__VA_ARGS__, const char* dbg_arg1, const char* dbg_arg2, const char* dbg_arg3, const char* dbg_func)

#define STR(X) #X

#else

#define DEBUG_LOG(f, ...)
#define DEBUG_LOG_F(file, f, ...)
#define DEBUG_FLUSH()
#define DEBUG_START()
#define DEBUG_STOP()

#define DEBUG(release, debug) release

#define DEBUGIZE_0(ret, func, ...) ret func(__VA_ARGS__)
#define DEBUGIZE_1(ret, func, ...) ret func(__VA_ARGS__)
#define DEBUGIZE_2(ret, func, ...) ret func(__VA_ARGS__)
#define DEBUGIZE_3(ret, func, ...) ret func(__VA_ARGS__)

#endif

#endif
