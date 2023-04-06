#include "core/io/file_io.h"
#include "math/math_inc.h"

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>

int file_io_check_exists_dbg(const char* file_path, const char* _file, const int _line)
{
  bool exists = true;
  FILE* f = fopen(file_path, "rb");
  if (f == NULL) 
  { exists = false; return false; }
  fclose(f);
  return exists;
}

char* file_io_read_dbg(const char* file_path, const char* _file, const int _line)
{
	printf("called read_text_file()\n");  // fucks this 
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    ERR_CHECK(f != NULL, "loading text-file at: %s \n  -> file: \"%s\", line: %d", file_path, _file, _line);

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    ASSERT(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    MALLOC(text, len * sizeof(char));
    
    // fill text buffer
    fread(text, 1, len, f);
    ASSERT(strlen(text) > 0);
    fclose(f);

    printf("text: %s\n", text); // fucks this then

    return text;
}
char* file_io_read_len_dbg(const char* file_path, int* length, const char* _file, const int _line)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    ERR_CHECK(f != NULL, "loading text-file at: %s \n  -> file: \"%s\", line: %d", file_path, _file, _line);

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    ASSERT(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    MALLOC(text, len * sizeof(char));

    // fill text buffer
    fread(text, sizeof(char), len, f);
    ASSERT(strlen(text) > 0);
    fclose(f);

    *length = len;
    return text;
}
u8* file_io_read_bytes_dbg(const char* file_path, int* length, const char* _file, const int _line)
{
    FILE* f;
    u8* text;
    long len;

    f = fopen(file_path, "rb");
    ERR_CHECK(f != NULL, "loading text-file at: %s \n  -> file: \"%s\", line: %d", file_path, _file, _line);

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    ASSERT(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    MALLOC(text, len * sizeof(char));

    // fill text buffer
    fread(text, sizeof(char), len, f);
    fclose(f);

    *length = len;
    return text;
}
void file_io_write_dbg(const char* file_path, const char* txt, int len, const char* _file, const int _line)
{
    FILE* f;

    f = fopen(file_path, "wb");
    ERR_CHECK(f != NULL, "loading text-file at: %s \n  -> file: \"%s\", line: %d", file_path, _file, _line);

    int rtn = fwrite(txt, sizeof(char), len, f);
    ASSERT(rtn != EOF);
    // fprintf(f, "%s", txt);

    fclose(f);
}
void file_io_write_bytes_dbg(const char* file_path, const u8* data, int len, const char* _file, const int _line)
{
    FILE* f;

    f = fopen(file_path, "wb");
    ERR_CHECK(f != NULL, "loading text-file at: %s \n  -> file: \"%s\", line: %d", file_path, _file, _line);

    int rtn = fwrite(data, sizeof(u8), len, f);
    ASSERT(rtn != EOF);
    // fprintf(f, "%s", txt);

    fclose(f);
}

void file_io_search_dir_for_file_type(const char* dir_path, const char* ext, search_dir_return_f* return_f)
{
  char path[256];
  struct dirent* dp;
  DIR* dir = opendir(dir_path);
  // unable to open directory stream
  if (!dir) { return; }
  

  // recursively read the directory and its sub-directories
  while ((dp = readdir(dir)) != NULL)
  {
    // check that the file currently read isn't a directory
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
    {
      if (strlen(dp->d_name) <= strlen(ext)) { return;}

      char* file_ending = dp->d_name + strlen(dp->d_name) - strlen(ext);
      if (strcmp(file_ending, ext) == 0)
      {
        char buf[300];
        // add slash if missing
        char* slash = dir_path[strlen(dir_path) -1] == '/'  ? "" :
                      dir_path[strlen(dir_path) -1] == '\\' ? "" : "\\";
        SPRINTF(300, buf, "%s%s%s", dir_path, slash, dp->d_name);
        
        return_f(buf, dp->d_name);
      }

      // construct new path from our base path
      strcpy(path, dir_path);
      path[strlen(path) -1] = '\0';
      strcat(path, "\\");
      strcat(path, dp->d_name);
      
      file_io_search_dir_for_file_type(path, ext, return_f);
    }
  }

  // close the stream
  closedir(dir);
}

