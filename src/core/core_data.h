#ifndef CORE_DATA_H
#define CORE_DATA_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"
#include "core/terrain.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


typedef struct core_data_t
{
  bool program_quit;  // if true program shuts down

  // -- window --

  GLFWmonitor* monitor;
  GLFWwindow*  window;

  f32 t_last_frame;
  f32 delta_t;
  f32 cur_fps;

  // -- renderer --

  bool wireframe_mode_enabled;
  bool show_shadows;
  
  framebuffer_t fb_deferred;
  framebuffer_t fb_lighting;
  framebuffer_t fb_shadow_pass;
  framebuffer_t fb_mouse_pick;

  shader_t basic_shader;
  shader_t shadow_shader;
  shader_t deferred_shader;
  shader_t lighting_shader;
  shader_t shadow_pass_shader;
  shader_t skybox_shader;
  shader_t post_fx_shader;
  shader_t brdf_lut_shader;       // for rendering the brdf look up texture
  shader_t mouse_pick_shader;
  
  cubemap_t cube_map;

  // -- input --

  f64 mouse_x;
  f64 mouse_y;
  f64 mouse_delta_x;
  f64 mouse_delta_y;
  
  f64 scroll_x;
  f64 scroll_y;
  f64 scroll_delta_x;
  f64 scroll_delta_y;

  // -- assetm --

  shader_t equirect_shader;       // for rendering equirectangular images to cube maps
  shader_t irradiance_map_shader; // for rendering the irradiance map from the cube map
  shader_t prefilter_shader;      // for rendering the prefilter map from the cube map
 
  // -- terrain --

  shader_t   terrain_shader;
  int*       terrain_materials;     // idx's for assetm
  u32        terrain_materials_len; 
  terrain_chunk_t* terrain_chunks;
  u32  terrain_chunks_len;
  f32  terrain_scl;
  f32  terrain_y_scl;
  // vec3 terrain_chunk_scl; // { terrain_scl, terrain_y_scl, terrain_scl }
  u32  terrain_x_len;
  u32  terrain_z_len;
  terrain_layout_t* terrain_layout;
  u32 terrain_layout_len;
  u32 terrain_draw_dist;
  u32 terrain_cull_dist;

  // -- camera --
  // ?
  // -- state --
  // ?
  // -----------

// #ifdef EDITOR
  bool phys_act;
  bool scripts_act;
// #endif

}core_data_t;

#define CORE_DATA_INIT()            \
{                                   \
  .program_quit = false,            \
                                    \
  .monitor = NULL,                  \
  .window  = NULL,                  \
                                    \
  .t_last_frame = 0.0f,             \
  .delta_t = 0.0f,                  \
  .cur_fps = 0.0f,                  \
                                    \
  .wireframe_mode_enabled = false,  \
  .show_shadows  = true,            \
                                    \
  .mouse_x = 0,                     \
  .mouse_y = 0,                     \
  .mouse_delta_x = 0,               \
  .mouse_delta_y = 0,               \
  .scroll_x = 0,                    \
  .scroll_y = 0,                    \
  .scroll_delta_x = 0,              \
  .scroll_delta_y = 0,              \
                                    \
  .terrain_materials = NULL,        \
  .terrain_materials_len = 0,       \
  .terrain_chunks = NULL,           \
  .terrain_chunks_len = 0,          \
  .terrain_scl   = 100,             \
  .terrain_y_scl = 0.02f / 256.0f,  \
  .terrain_x_len = 100,             \
  .terrain_z_len = 100,             \
  .terrain_layout = NULL,           \
  .terrain_layout_len = 0,          \
  .terrain_draw_dist = 2,           \
  .terrain_cull_dist = 3,           \
                                    \
  .phys_act = true,                 \
  .scripts_act = true,              \
}


// -- func decls --

core_data_t* core_data_get();
void core_data_init();

#endif
