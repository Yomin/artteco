/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_RUBOUT__
#define __ART_TECO_RUBOUT__

typedef void rubout_func();

void rubout_init();
void rubout_finish();
void rubout_register_s(rubout_func* f);
void rubout_register(rubout_func* f, void* data, int size);
void rubout_save(void* data, int size);
int  rubout_load(void* data);
int  rubout_info();
void rubout_start();
void rubout_end();
void rubout_break();
void rubout_clear();
void rubout();

#endif
