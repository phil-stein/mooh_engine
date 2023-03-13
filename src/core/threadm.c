#include "core/threadm.h"
#include "core/core_data.h"
#include "core/io/file_io.h"
#include "core/io/asset_io.h"
#include "core/debug/debug_opengl.h"

#include "stb/stb_ds.h"

// @TODO: fix vs-style pragmas
// @TODO: thread_timer_init() & thread_timer_term()
//        commented out because missing timeGetDevCaps()
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#define THREAD_IMPLEMENTATION
#include "thread/thread.h"
#pragma GCC diagnostic pop

#define THREADM_NAME_MAX 64
char name_buffer[THREADM_NAME_MAX];

u32 total_thread_count = 0;  // 0 means just main thread no extra threads

static core_data_t* core_data = NULL;

// -- generic --

void threadm_init()
{
  core_data = core_data_get();
}

void threadm_join(thread_t* thread)
{
  total_thread_count = thread_join(thread->ptr);
}

void threadm_destroy(thread_t* thread)
{
  thread_destroy(thread->ptr);
}

// -- specific --

#define THREAD_MAX 4

void threadm_load_texture_arr(texture_load_data_t** tex_arr_ptr, u32* tex_arr_len_ptr)
{
  // need them as pointers, as they have to be reset at end of func
  texture_load_data_t* tex_arr = *tex_arr_ptr;
  u32 tex_arr_len = *tex_arr_len_ptr;
  if (tex_arr == NULL || tex_arr_len <= 0) { return; }

  thream_load_tex_args_t args_arr[THREAD_MAX];
  thread_t thread_arr[THREAD_MAX];
  u32 thread_arr_len = 0;
  u32 tex_idx = 0;
  while (tex_idx < tex_arr_len) // keep doing X threads / textures at a time until all are done
  {
    for (u32 i = 0; i < THREAD_MAX && tex_idx < tex_arr_len; ++i) // start X textures
    {
      // PF("[threadm] start loaded \"%s\"\n", tex_arr[tex_idx].name);
      thread_arr[i] = threadm_start_load_texture_file(tex_arr[tex_idx].name, tex_arr[tex_idx].srgb, &args_arr[i]);
      thread_arr_len++;
      tex_idx++;
    }
    // wait till all threads finished
    for (u32 i = 0; i < thread_arr_len; ++i) 
    {
      threadm_join(&thread_arr[i]);
    }
    ASSERT(total_thread_count == 0);  // 0 means only main thread
    
    // use data to register with opengl
    u32 i = 0;
    for (i = 0; i < thread_arr_len; ++i)  // create the X loaded textures
    {
      thream_load_tex_args_t* args = &args_arr[i]; // (thream_load_tex_args_t*)thread_arr[i].data;
      u32 handle = texture_create_from_pixels(args->pixels, (size_t)args->w, (size_t)args->h, (int)args->channels, args->srgb); 
      free(args_arr[i].buffer);
      texture_t t;
      t.handle = handle;
      t.width = args->w;
      t.height = args->h;
      // tex_idx -thread_arr_len +i:
      //    e.g. thread_arr_len is 4
      //    tex_idxs gets increased +4 in first for loop
      //    i is 0 - 3
      //    so -4 +1 is the same +0, +1, +2, +3, that the first loop has
      //    which wrote the data were accesing
      assetm_overwrite_texture_idx(tex_arr[tex_idx -thread_arr_len +i].idx, &t); 
    }
    
    for (u32 i = 0; i < thread_arr_len; ++i)
    {
      threadm_destroy(&thread_arr[i]);
    }
    thread_arr_len = 0;
    
    // for (u32 i = 0; i < THREAD_MAX; ++i)
    // {
    //   if (args_arr[i].buffer != NULL) { free(args_arr[i].buffer); }
    //   args_arr[i].buffer = NULL;
    // }
  }

  // free the passed array
  arrfree(tex_arr);
  *tex_arr_ptr = NULL;
  *tex_arr_len_ptr = 0;
}


thread_t threadm_start_load_texture_file(char* name, bool srgb, thream_load_tex_args_t* args)
{
  args->w = 0;
  args->h = 0;
  args->channels = 0;
  args->srgb     = srgb;
  args->buffer   = NULL;
  strcpy(args->name, name);

  thread_t t;
  t.func = threadm_load_tex_file_f;
  t.data = (void*)args;
  t.ptr  = thread_create(threadm_load_tex_file_f, (void*)args, THREAD_STACK_SIZE_DEFAULT);
  total_thread_count++;
  
  return t; 
}
int threadm_load_tex_file_f(void* data)
{
  thream_load_tex_args_t* args = (thream_load_tex_args_t*)data;

  u32 i = 0; while(args->name[i] != '.' && args->name[i +1] != '\0') 
  { name_buffer[i] = args->name[i]; i++; } name_buffer[i] = '\0';
  int length = 0;
  char path[ASSET_PATH_MAX + THREADM_NAME_MAX + 12];
  sprintf(path, "%stextures/%s%s", core_data->asset_path, name_buffer, ".tex");
  args->buffer = file_io_read_bytes(path, &length);

  asset_io_deserialize_texture(args->buffer, &args->pixels, &args->w, &args->h, &args->channels);
  
  return 0; // like return code in main()
}

