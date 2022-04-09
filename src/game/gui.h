#ifdef  EDITOR

#ifndef GUI_H
#define GUI_H

#include "global/global.h"

// ---- util ----

typedef struct nk_context ui_context;
typedef struct nk_glfw    ui_glfw;
typedef struct nk_rect    ui_rect;
typedef struct nk_image   ui_image;
typedef struct nk_color   ui_color;
typedef struct nk_colorf  ui_colorf;

// --------------


void gui_init();
void gui_update();
void gui_cleanup();

#endif

#endif
