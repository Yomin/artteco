/*    _________________________
//   / _ | _  / /_  __\  _\  _ \
//  / __ |   / __/ / -_\ \_\ \\ \
// /_/ |_|_\_\__/_/\___/\___\____\
*/

#ifndef __ART_TECO_FILE__
#define __ART_TECO_FILE__

#include "list.h"

#include <stdio.h>

#define FILE_LINE_SIZE 128
#define FILE_LINE_COUNT_SOFT 64     // up to 8KB
#define FILE_LINE_COUNT_HARD 128    // up to 16KB
#define FILE_CHUNK_COUNT 32         // up to 512KB

#define FILE_NAME_SIZE 80

#define FILE_ERROR_NOT_FOUND -1
#define FILE_ERROR_NAME_SIZE -2

struct file_line
{
    char line[FILE_LINE_SIZE+1];
    int size;
};

struct file_chunk
{
    int start, end;
    struct list_state lines;
};

struct file_state
{
    FILE* file;
    char name[FILE_NAME_SIZE];
    struct list_state chunks;
};

void file_init(struct file_state* file);
void file_close(struct file_state* file);
int  file_load(const char* filename, struct file_state* file);

#endif
