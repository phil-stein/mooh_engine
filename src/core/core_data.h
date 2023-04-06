#ifndef CORE_DATA_H
#define CORE_DATA_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"
#include "core/terrain.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

// @TODO: define this in make and use in all of core
// #define INCLUDE_PLAY_MODE


// @TODO: replace phys_act & scripts_act with flag

// @DOC: maximun length of the ASSET_PATH macro defined in make
#define ASSET_PATH_MAX 256
// @DOC: maximun length of the path to the shaders aka. ASSET_PATH/shaders
#define SHADERS_PATH_MAX ASSET_PATH_MAX

typedef struct core_data_t
{
  bool program_quit;  // if true program shuts down

  // -- window --

  GLFWmonitor* monitor;
  GLFWwindow*  window;

  f32 t_last_frame;  // time it took to run the last frame
  f32 delta_t;       // how much time has passed since last frame
  f32 cur_fps;       // frames per second

  // -- assetm --
  
  bool use_async_asset_arrs;    // use assetm_register_...() and threadm_load_..._arr(), or load direct

  char asset_path[ASSET_PATH_MAX];
  char shaders_path[SHADERS_PATH_MAX];
  
  shader_t equirect_shader;       // for rendering equirectangular images to cube maps
  shader_t irradiance_map_shader; // for rendering the irradiance map from the cube map
  shader_t prefilter_shader;      // for rendering the prefilter map from the cube map
  
  // -- camera --
  
  camera_t cam;

  // -- renderer_direct --
  
  // created in core_data_init_renderer()
  u32 quad_vao, quad_vbo;     // 
  int quad_mesh;
  mesh_t line_mesh;

  // -- renderer --
 
  // after renderer_update() has run this is all draw calls
  u32 draw_calls_total; 
  u32 draw_calls_shadow;     
  u32 draw_calls_deferred;
  u32 draw_calls_screen_quad;

  bool wireframe_mode_enabled;
  bool show_shadows;

  u32 brdf_lut;

  // created in core_data_init_renderer()
  framebuffer_t fb_deferred_msaa;
  framebuffer_t fb_deferred;
  framebuffer_t fb_lighting;
  framebuffer_t fb_shadow_pass;
  framebuffer_t fb_mouse_pick;
  framebuffer_t fb_outline;

  // created in core_data_init_renderer()
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
 
  // -- renderer_extra --

  int outline_id; // used in renderer_extra_draw_outline()

  // -- input --

  f64 mouse_x;
  f64 mouse_y;
  f64 mouse_delta_x;
  f64 mouse_delta_y;
  
  f64 scroll_x;
  f64 scroll_y;
  f64 scroll_delta_x;
  f64 scroll_delta_y;
 
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

  // -- state --
  // ?
  // -----------

  // @TODO: stripping out when not defined 
  //        const doesnt work
#ifdef INCLUDE_PLAY_MODE
// #ifdef EDITOR
  bool phys_act;
  bool phys_debug_act;
  bool scripts_act;
#else 
  bool phys_act;
  bool phys_debug_act;
  bool scripts_act;
#endif

}core_data_t;

// value for phys_act & scripts_act
#ifdef INCLUDE_PLAY_MODE
#define PLAY_ACT_VALUE false
#else
#define PLAY_ACT_VALUE true
#endif

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
  .use_async_asset_arrs = false,              \
  .asset_path   = "\0",                       \
  .shaders_path = "\0",                       \
                                              \
  .cam.pos     = { 0, 0, 0 },                 \
  .cam.front   = { 0, 0, -1 },                \
  .cam.up      = { 0, 1, 0 },                 \
  .cam.target  = { 0, 0, 0 },                 \
  .cam.fov     = 45.0f,                       \
  .cam.fov_rad = 45.0f * M_PI_F / 180.0f,     \
  .cam.near_plane  = 0.1f,                    \
  .cam.far_plane   = 1000.0f,                 \
                                              \
  .quad_mesh = -1,                            \
                                              \
  .draw_calls_total       = 0,                \
  .draw_calls_screen_quad = 0,                \
  .draw_calls_deferred    = 0,                \
  .draw_calls_shadow      = 0,                \
                                              \
  .wireframe_mode_enabled = false,            \
  .show_shadows  = true,                      \
                                              \
  .brdf_lut = -1,                             \
                                              \
  .outline_id  = -1,                          \
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
  .phys_act       = PLAY_ACT_VALUE,           \
  .phys_debug_act = false,                    \
  .scripts_act    = PLAY_ACT_VALUE,           \
}



// -- func decls --

// @DOC: initialize core_data call this before any other calls to core_data
void core_data_init();

// @DOC: get pointer to core_data struct
core_data_t* core_data_get();

#ifdef INCLUDE_PLAY_MODE
// play or pause the game, also saving/restoring game state
void core_data_play_func();
void core_data_play_scripts_func();
void core_data_play_phys_func();
void core_data_pause_func();

// returns phys_act || scripts_act
bool core_data_is_play_func();

#define core_data_is_play()        core_data_is_play_func()
#define core_data_play()           core_data_play_func()
#define core_data_play_scripts()   core_data_play_scripts_func()   
#define core_data_play_phys()      core_data_play_phys_func() 
#define core_data_pause()          core_data_pause_func()  

#else

#define core_data_is_play()        true 
#define core_data_play()           
#define core_data_play_scripts()  
#define core_data_play_phys()     
#define core_data_pause()         

#endif

#endif
