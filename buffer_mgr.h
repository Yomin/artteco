/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_BUFFER_MGR__
#define __ART_TECO_BUFFER_MGR__

#include "buffer.h"

#define BUFFER_DELETE_NOTFOUND  0
#define BUFFER_DELETE_FOUND     1
#define BUFFER_DELETE_FORBIDDEN 2

void buffer_mgr_init();
void buffer_mgr_finish();

struct buffer_state* buffer_mgr_add(const char* name);
struct buffer_state* buffer_mgr_add_intern(const char* name);
struct buffer_state* buffer_mgr_add_file(const char* name, const char* file);
int buffer_mgr_delete(int number);
struct buffer_state* buffer_mgr_current();
struct buffer_state* buffer_mgr_switch(int number);

#endif
