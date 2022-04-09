#ifndef FILE_IO_H
#define FILE_IO_H

#include "global/global.h"
#include "core/types/mesh.h"

// returns true if the file under the specified path "file_path" exists, otherwise false
int file_check_exists(const char* file_path);

// read a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* file_read(const char* file_path);
// read the given lengt of a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* file_read_len(const char* file_path, int* length);

// read binary file
u8* file_read_bytes(const char* file_path, int* length);

// writes text "txt" into file at "file_path", creates file if it doesnt exist
// "len" is the length of "txt", or shorter if you want to cut off the string
void file_write(const char* file_path, const char* txt, int len);

#endif
