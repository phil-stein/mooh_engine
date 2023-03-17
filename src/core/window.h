#ifndef WINDOW_H
#define WINDOW_H

#include "global/global.h"
#include "core/core_data.h"
#include "core/types/framebuffer.h"

// @DOC: max length of title str
#define WINDOW_TITLE_MAX 512

// @DOC: window modes minimized, maximized, fullscreen
typedef enum window_type { WINDOW_MIN, WINDOW_MAX, WINDOW_FULL } window_type;
#define P_WINDOW_TYPE(a)  { PF("%s\n", (a) == WINDOW_MIN ? "WINDOW_MIN" : (a) == WINDOW_MAX ? "WINDOW_MAX" : (a) == WINDOW_FULL ? "WINDOW_FULL" : "?UNKNOWN?"); }

// ---- func decls ----

// @DOC: creates a glfw window
//       set refresh rate to <= 0 to choose native monitor refresh rate
//       width:  width of window in pixel
//       height: height of window in pixel
//       title:  title of window, max length is WINDOW_TITLE_MAX
//       type:   type to be initially
bool window_create(const int width, const int height, const char* title, window_type type);

// @DOC: get current size of window
//       w: gets set to width
//       h: gets set to height
void window_get_size(int* w, int* h);
// @DOC: get the current window mode
window_type window_get_mode();
// @DOC: get the current window title
//       to change it call window_set_title()
char* window_get_title(); 
// @DOC: get glfw bool indicating if the widow should close
bool window_get_should_close();
// @DOC: get size of the screen the window in on in cm
//       w: gets set to width in cm
//       h: gets set to height in cm
void  window_get_monitor_size_cm(float* w, float* h);
// @DOC: gets the dpi of the screen the window in on
//       dpi is pixel per inch
//       w: gets set to dpi width
//       h: gets set to dpi height
void  window_get_monitor_dpi(float* w, float* h);

// @DOC: close window
void window_close();

// @DOC: register a framebuffer to get its size set to the windows size on resize
//       fb: framebuffer to be registered
void window_set_texturebuffer_to_update_to_screen_size(framebuffer_t* fb);

// @DOC: set the windows title
//       max length is WINDOW_TITLE_MAX
//       title: new title of window
void window_set_title(const char* title);
// @DOC: set the windows mode
//       type: the mode to be switched to
void window_set_mode(window_type type);


#endif
