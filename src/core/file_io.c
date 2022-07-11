#include "core/file_io.h"
#include "math/math_inc.h"

#define STB_DS_IMPLEMENTATION       // only define once
#include "stb/stb_ds.h"         

#include <direct.h>

int file_check_exists(const char* file_path)
{
  bool exists = true;
  FILE* f = fopen(file_path, "rb");
  if (f == NULL) 
  { exists = false; }
  fclose(f);
  return exists;
}

char* file_read(const char* file_path)
{
	printf("called read_text_file()\n");
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    text = calloc(1, len * sizeof(char));
    assert(text != NULL);
    
    // fill text buffer
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    printf("text: %s\n", text);

    return text;
}
char* file_read_len(const char* file_path, int* length)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    text = calloc(1, len);
    assert(text != NULL);

    // fill text buffer
    fread(text, sizeof(char), len, f);
    assert(strlen(text) > 0);
    fclose(f);

    *length = len;
    return text;
}
u8* file_read_bytes(const char* file_path, int* length)
{
    FILE* f;
    u8* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    text = calloc(1, len);
    assert(text != NULL);

    // fill text buffer
    fread(text, sizeof(char), len, f);
    fclose(f);

    *length = len;
    return text;
}
void file_write(const char* file_path, const char* txt, int len)
{
    FILE* f;

    f = fopen(file_path, "wb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    int rtn = fwrite(txt, sizeof(char), len, f);
    assert(rtn != EOF);
    // fprintf(f, "%s", txt);

    fclose(f);
}


