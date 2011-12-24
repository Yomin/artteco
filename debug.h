/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_DEBUG__
#define __ART_TECO_DEBUG__

#ifndef NDEBUG

#define DEBUG_LOG(f, ...)   debug_log(__func__, f, ##__VA_ARGS__)
#define DEBUG_FLUSH()       debug_flush(__func__)
#define DEBUG_START()       debug_start()
#define DEBUG_STOP()        debug_stop()

void debug_start();
void debug_stop();
void debug_log(const char* func, const char* format, ...);
void debug_flush();

#else

#define DEBUG_LOG(f, ...)
#define DEBUG_FLUSH()
#define DEBUG_START()
#define DEBUG_STOP()

#endif

#endif
