/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_DEBUG__
#define __ART_TECO_DEBUG__

#ifndef NDEBUG

#include <stdio.h>

#define DEBUG_LOG(f, ...)           debug_log(__func__, f, ##__VA_ARGS__)
#define DEBUG_LOG_F(file, f, ...)   debug_log(__func__, file, f, ##__VA_ARGS__)
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
