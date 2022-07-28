#ifdef  EDITOR

#ifndef GUI_H
#define GUI_H

#include "global/global.h"
#include "core/types/types.h"
#include "data/entity_template.h"
#include "math/math_inc.h"

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


void gui_top_bar_win();

void gui_hierarchy_win();
void gui_hierarchy_display_entity_and_children(entity_t* e, int* offs);


void gui_light_hierarchy_win();

void gui_framebuffer_win();

void gui_debug_win();
void gui_shader_properties(shader_t* s, char* name);

void gui_core_data_win();


void gui_color_selector(rgbf color);

#endif

#endif
