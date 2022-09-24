#ifndef CORE_DATA_H
#define CORE_DATA_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"
#include "core/terrain.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


// @TODO: replace phys_act & scripts_act with flag

typedef struct core_data_t
{
  bool program_quit;  // if true program shuts down

  // -- window --

  GLFWmonitor* monitor;
  GLFWwindow*  window;

  f32 t_last_frame;
  f32 delta_t;
  f32 cur_fps;

  // -- camera --
  vec3 cam_pos;
  vec3 cam_front;
  vec3 cam_up;
  vec3 cam_target;
  const f32 cam_fov; 
  const f32 cam_fov_rad;     
  const f32 near_plane; 
  const f32 far_plane;  


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

#define CORE_DATA_INIT()                      \
{                                             \
  .program_quit = false,                      \
                                              \
  .monitor = NULL,                            \
  .window  = NULL,                            \
                                              \
  .t_last_frame = 0.0f,                       \
  .delta_t = 0.0f,                            \
  .cur_fps = 0.0f,                            \
                                              \
  .cam_pos     = { 0, 0, 0 },                 \
  .cam_front   = { 0, 0, -1 },                \
  .cam_up      = { 0, 1, 0 },                 \
  .cam_target  = { 0, 0, 0 },                 \
  .cam_fov     = 45.0f,                       \
  .cam_fov_rad = 45.0f * M_PI_F / 180.0f,     \
  .near_plane  = 0.1f,                        \
  .far_plane   = 1000.0f,                     \
                                              \
  .wireframe_mode_enabled = false,            \
  .show_shadows  = true,                      \
                                              \
  .mouse_x = 0,                               \
  .mouse_y = 0,                               \
  .mouse_delta_x = 0,                         \
  .mouse_delta_y = 0,                         \
  .scroll_x = 0,                              \
  .scroll_y = 0,                              \
  .scroll_delta_x = 0,                        \
  .scroll_delta_y = 0,                        \
                                              \
  .terrain_materials = NULL,                  \
  .terrain_materials_len = 0,                 \
  .terrain_chunks = NULL,                     \
  .terrain_chunks_len = 0,                    \
  .terrain_scl   = 100,                       \
  .terrain_y_scl = 0.02f / 256.0f,            \
  .terrain_x_len = 100,                       \
  .terrain_z_len = 100,                       \
  .terrain_layout = NULL,                     \
  .terrain_layout_len = 0,                    \
  .terrain_draw_dist = 2,                     \
  .terrain_cull_dist = 3,                     \
                                              \
  .phys_act = false,                          \
  .scripts_act = false,                       \
}



// -- func decls --

core_data_t* core_data_get();
void core_data_init();

// play or pause the game, also saving/restoring game state
void core_data_play();
void core_data_play_scripts();
void core_data_play_phys();
void core_data_pause();

// returns phys_act || scripts_act
bool core_data_is_play();

#endif
