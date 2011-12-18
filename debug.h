/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_DEBUG__
#define __ART_TECO_DEBUG__

#ifndef NDEBUG

#define LOG(msg)        debug_log(__func__, msg)
#define LOGF(f, var)    debug_logf(__func__, f, var)
#define DEBUG_START()   debug_start()
#define DEBUG_STOP()    debug_stop()

void debug_start();
void debug_stop();
void debug_log(const char* func, const char* msg);
void debug_logf(const char* func, const char* format, int var);

#else

#define LOG(msg)
#define DEBUG_START()
#define DEBUG_STOP()

#endif

#endif
