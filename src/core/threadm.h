#ifndef CORE_THREADM_H
#define CORE_THREADM_H

#include "global/global.h"
#include "core/io/assetm.h"

#include "thread/thread.h"

// type of func that can be passed into thread_create()
typedef int (thread_f)(void* data); 

// @DOC: all data associated with a thread
typedef struct
{
  thread_ptr_t ptr;
  thread_f*    func;
  void*        data;
}thread_t;

#define THREAM_NAME_MAX 64
// @DOC: struct used to fill in another thread and pass back to main thread
typedef struct
{
  char name[THREAM_NAME_MAX];
  u8* pixels; // pointer into buffer
  u32 w, h, channels;
  bool srgb;

  u8* buffer; // needs to be freed
}thream_load_tex_args_t;


// -- generic --

// @DOC: initialize, call this before any other calls to threadm
void threadm_init();
// @UNSURE: necessary ?
// thread_t threadm_create(func, data);
// @DOC: wait for thread to finish, then continue
void threadm_join(thread_t* thread);
// @DOC: end a thread
void threadm_destroy(thread_t* thread);

// -- special --

// @DOC: loads the textures define in tex_arr asynchronous
//       ! tex_arr is std_ds array, gets arrfree()'d
//       ! tex_arr_len_ptr gets set to 0
//       tex_arr_ptr:     pointer to array with info of the textures to be loaded
//       tex_arr_len_ptr: pointer to array length
void threadm_load_texture_arr(texture_load_data_t** tex_arr_ptr, u32* tex_arr_len_ptr);
// @DOC: starts a thread to load file data from a '.tex' file
//       name: name of texture file
//       srgb: if the texture should be loaded as srgb for hdr
//       args: gets filled with the loaded data
thread_t threadm_start_load_texture_file(char* name, bool srgb, thream_load_tex_args_t* args);
// @DOC: func invoced by threadm_start_load_texture_file() to open thread
//       ! data.name needs to be filled before calling func
//       data: thream_load_tex_args_t* to be filled with data
int threadm_load_tex_file_f(void* data);


#endif // THREADM_H
