#ifndef FILE_IO_H
#define FILE_IO_H

#include "global/global.h"
#include "core/types/mesh.h"

// @DOC: returns true if the file under the specified path "file_path" exists, otherwise false
//       file_path: path to the file, including file name
int file_io_check_exists_dbg(const char* file_path, const char* _file, const int _line);
#define file_io_check_exists(p)   file_io_check_exists_dbg((p), __FILE__, __LINE__)

// @DOC: read a text-file and output all contents as a char*
//       !!! free() the returned char* as it gets allocated
//       taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
//       file_path: path to the file, including file name
char* file_io_read_dbg(const char* file_path, const char* _file, const int _line);
#define file_io_read(p)   file_io_read_dbg((p), __FILE__, __LINE__)

// @DOC: read the given lengt of a text-file and output all contents as a char*
//       !!! free() the returned char* as it gets allocated
//       taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
//       file_path: path to the file, including file name
//       length:    gets set to the length of the returned file
char* file_io_read_len_dbg(const char* file_path, int* length, const char* _file, const int _line);
#define  file_io_read_len(file_path, length) file_io_read_len_dbg((file_path), (length), __FILE__, __LINE__)

// @DOC: read binary file
//       file_path: path to the file, including file name
//       length:    gets set to the length of the returned file
u8* file_io_read_bytes_dbg(const char* file_path, int* length, const char* _file, const int _line);
#define file_io_read_bytes(path, len) file_io_read_bytes_dbg((path), (len), __FILE__, __LINE__)

// @DOC: writes text 'txt' into file at 'file_path', creates file if it doesnt exist
//       file_path: path to the file, including file name
//       txt:       text to be written
//       len:       the length of 'txt', or shorter if you want to cut off the string
void file_io_write_dbg(const char* file_path, const char* txt, int len, const char* _file, const int _line);
#define file_io_write(p, txt, l)  file_io_write_dbg((p), (txt), (l), __FILE__, __LINE__)

// @DOC: writes bytes 'data' into file at 'file_path', creates file if it doesnt exist
//       file_path: path to the file, including file name
//       data:      bytes to be written
//       len:       the length of 'txt', or shorter if you want to cut off the string
void file_io_write_bytes_dbg(const char* file_path, const u8* data, int len, const char* _file, const int _line);
#define file_io_write_bytes(p, d, l)    file_io_write_bytes_dbg((p), (d), (l), __FILE__, __LINE__)

#endif
