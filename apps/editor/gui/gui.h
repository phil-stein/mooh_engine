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

#define P_NK_COLOR(c)  PF_COLOR(PF_CYAN);PF("%s: ", #c); PF_COLOR(PF_WHITE); PF("r: %d, g: %d, b: %d, a: %d\n", c.r, c.g, c.b, c.a)

// --------------

// @DOC: initialize, call this before any other calls to gui
void gui_init();
// @DOC: update gui logic, call once a frame
void gui_update();
// @DOC: free allocated resources, call before exiting program
void gui_cleanup();

// @DOC: bar at the top with scen, window & control menus
// void gui_top_bar_win();
void gui_top_bar_win(ui_context* _ctx, ui_rect win_rect, const u32 win_flags); 

// @DOC: window displaying all properties of an entity_t
// void gui_properties_win();
void gui_properties_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags, bool is_selected);
// @DOC: not window, for use in one, displays transform
void gui_properties_transform(ui_context* ctx, entity_t* e, vec3 pos, vec3 rot, vec3 scl, bool* has_moved);
// @DOC: not window, for use in one, displays material
void gui_properties_material(ui_context* ctx, ui_rect win_rect, material_t* mat, int idx);
// @DOC: not window, for use in one, displays mesh
void gui_properties_mesh(ui_context* ctx, mesh_t* m, int idx, int entity_template_idx);
// @DOC: not window, for use in one, displays point_light
void gui_properties_point_light(ui_context* ctx, point_light_t* p, int idx); // (vec3 pos, rgbf col, f32 intensity);
// @DOC: not window, for use in one, displays physics properties
void gui_properties_physics(ui_context* ctx, const entity_template_t* def, entity_t* e);

// @DOC: window at the bottom, to add entities from templates
void gui_template_browser_win();

// @DOC: window showing a hierarchy of all entities
void gui_hierarchy_win();
// @DOC: used by gui_hierarchy_win(), displays entity and its children
void gui_hierarchy_display_entity_and_children(entity_t* e, int* offs);

// @DOC: shows all dir/point_lights and lets you control the cubemap intensity
void gui_light_hierarchy_win();

// @DOC: shows all active framebuffers
void gui_framebuffer_win();

// @DOC: displays debug info, like all timers
void gui_debug_win();

// @DOC: not window, for use in one, displays a shader's properties
void gui_shader_properties(shader_t* s, char* name);

// @DOC: window displaying all variables in core_data_t
void gui_core_data_win();

// @DOC: not window, for use in one, color selector
void gui_color_selector(rgbf color);


// ---external---

void gui_struct_browser_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags);

#endif
