/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_RUBOUT__
#define __ART_TECO_RUBOUT__

#include "debug.h"

#define RUBOUT_SUCCESS 0
#define RUBOUT_FAILURE 1

typedef void rubout_func();

void rubout_init();
void rubout_finish();
DEBUGIZE_1(int, rubout_register_s, rubout_func* f);
DEBUGIZE_3(int, rubout_register, rubout_func* f, void* data, int size);
DEBUGIZE_2(void, rubout_save, void* data, int size);
int  rubout_load(void* data);
int  rubout_info();
void rubout_start();
void rubout_end();
void rubout_break();
void rubout_clear();
void rubout();
int rubout_ptr_register(rubout_func* f);

#ifndef NDEBUG
    #define rubout_register_s(f) rubout_register_s_dbg(f, STR(f), __func__)
    #define rubout_register(f, data, size) rubout_register_dbg(f, data, size, STR(f), STR(data), STR(size), __func__)
    #define rubout_save(data, size) rubout_save_dbg(data, size, STR(data), STR(size), __func__)
#endif

#endif
