#ifndef WINDOW_H
#define WINDOW_H

#include "global/global.h"
#include "core/core_data.h"
#include "core/types/framebuffer.h"

#define WINDOW_TITLE_MAX 512

typedef enum window_type { WINDOW_MIN, WINDOW_MAX, WINDOW_FULL } window_type;
#define P_WINDOW_TYPE(a)  { PF("%s\n", (a) == WINDOW_MIN ? "WINDOW_MIN" : (a) == WINDOW_MAX ? "WINDOW_MAX" : (a) == WINDOW_FULL ? "WINDOW_FULL" : "?UNKNOWN?"); }

// ---- func decls ----
// creates a glfw window
// set refresh rate to <= 0 to choose native monitor refresh rate
bool window_create(const int width, const int height, const char* title, window_type type);

// void* window_get();
void window_get_size(int* w, int* h);
window_type window_get_mode();
char* window_get_title(); 
bool window_get_should_close();
void  window_get_monitor_size_cm(float* w, float* h);
void  window_get_monitor_dpi(float* w, float* h);

void window_close();
void window_set_texturebuffer_to_update_to_screen_size(framebuffer_t* fb);

void window_set_title(const char* title);
void window_set_mode(window_type type);


#endif
