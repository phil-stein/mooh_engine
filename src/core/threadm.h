#ifndef THREADM_H
#define THREADM_H

#include "global/global.h"

typedef int (thread_f)(void* data); 

// @UNSURE: just use thread.h directly

typedef struct
{
  thread_ptr_t ptr;
  thread_f     func;
  void*        data;
}thread_t;

// -- generic --

thread_t threadm_create(func, data);
void threadm_join(thread_t thread);
void threadm_destroy(thread_t thread);

// -- special --

thread_t threadm_start_load_texture(char* name);

#endif // THREADM_H
