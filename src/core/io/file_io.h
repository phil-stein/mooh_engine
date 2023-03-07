#ifndef FILE_IO_H
#define FILE_IO_H

#include "../global/global.h"
#include "../core/types/mesh.h"

// @DOC: returns true if the file under the specified path "file_path" exists, otherwise false
//       file_path: path to the file, including file name
int file_io_check_exists(const char* file_path);

// @DOC: read a text-file and output all contents as a char*
//       !!! free() the returned char* as it gets allocated
//       taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
//       file_path: path to the file, including file name
char* file_io_read(const char* file_path);
// @DOC: read the given lengt of a text-file and output all contents as a char*
//       !!! free() the returned char* as it gets allocated
//       taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
//       file_path: path to the file, including file name
//       length:    gets set to the length of the returned file
char* file_io_read_len(const char* file_path, int* length);

// @DOC: read binary file
//       file_path: path to the file, including file name
//       length:    gets set to the length of the returned file
u8* file_io_read_bytes(const char* file_path, int* length);

// @DOC: writes text 'txt' into file at 'file_path', creates file if it doesnt exist
//       file_path: path to the file, including file name
//       txt:       text to be written
//       len:       the length of 'txt', or shorter if you want to cut off the string
void file_io_write(const char* file_path, const char* txt, int len);

// @DOC: writes bytes 'data' into file at 'file_path', creates file if it doesnt exist
//       file_path: path to the file, including file name
//       data:      bytes to be written
//       len:       the length of 'txt', or shorter if you want to cut off the string
void file_io_write_bytes(const char* file_path, const u8* data, int len);

#endif
