/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\ 
*/

#ifndef __ART_TECO_BUFFER_MGR__
#define __ART_TECO_BUFFER_MGR__

#include "buffer.h"

#define BUFFER_MGR_ERROR_ACCESS_FORBIDDEN -1
#define BUFFER_MGR_ERROR_FILE_NOT_FOUND   -2
#define BUFFER_MGR_ERROR_FILE_NAME_SIZE   -3
#define BUFFER_MGR_ERROR_NOT_FOUND        -4
#define BUFFER_MGR_ERROR_EXISTING         -5

void buffer_mgr_init();
void buffer_mgr_finish();

void buffer_mgr_add(const char* name);
void buffer_mgr_add_intern(const char* name);
int  buffer_mgr_add_file(const char* name, const char* file);
int  buffer_mgr_delete(int number);

struct buffer_state* buffer_mgr_current();
struct buffer_state* buffer_mgr_switch(int number);

#endif
