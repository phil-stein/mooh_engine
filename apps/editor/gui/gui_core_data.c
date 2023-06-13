#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "editor/gui/gui.h"
#include "editor/gui/gui_style.h"
// #include "editor/app.h"
#include "core/core_data.h"
#include "core/window.h"


void gui_core_data_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags)
{
  int w, h;
  window_get_size(&w, &h);
  
  core_data_t* core_data = core_data_get();;
  // app_data_t*  app_data  = app_data_get();;

  if (nk_begin(ctx, "core_data", win_rect, win_flags)) 
  {
    nk_layout_row_dynamic(ctx, 20, 1);
    nk_labelf(ctx, NK_TEXT_LEFT, "t_last_frame: %f", core_data->t_last_frame);   
    nk_labelf(ctx, NK_TEXT_LEFT, "delta_t: %f", core_data->delta_t);   
    nk_labelf(ctx, NK_TEXT_LEFT, "cur_fps: %f", core_data->cur_fps);
        
    nk_spacing(ctx, 1);
    nk_labelf(ctx, NK_TEXT_LEFT, "draw_calls_total:       %u", core_data->draw_calls_total);
    nk_labelf(ctx, NK_TEXT_LEFT, "draw_calls_shadow:      %u", core_data->draw_calls_shadow);
    nk_labelf(ctx, NK_TEXT_LEFT, "draw_calls_deferred:    %u", core_data->draw_calls_deferred);
    nk_labelf(ctx, NK_TEXT_LEFT, "draw_calls_screen_quad: %u", core_data->draw_calls_screen_quad);
    nk_spacing(ctx, 1);
    nk_checkbox_label(ctx, "wireframe_mode_enabled", &core_data->wireframe_mode_enabled);
    nk_checkbox_label(ctx, "show_shadows", &core_data->show_shadows);
   
    // framebuffers, shaders & cubemap
    nk_spacing(ctx, 1);
    gui_shader_properties(ctx, &core_data->basic_shader, "basic_shader");
    gui_shader_properties(ctx, &core_data->shadow_shader, "shadow_shader");
    gui_shader_properties(ctx, &core_data->deferred_shader, "deferred_shader");
    gui_shader_properties(ctx, &core_data->lighting_shader, "lighting_shader");
    gui_shader_properties(ctx, &core_data->shadow_pass_shader, "shadow_pass_shader");
    gui_shader_properties(ctx, &core_data->skybox_shader, "skybox_shader");
    gui_shader_properties(ctx, &core_data->post_fx_shader, "post_fx_shader");
    gui_shader_properties(ctx, &core_data->brdf_lut_shader, "brdf_lut_shader");
    gui_shader_properties(ctx, &core_data->mouse_pick_shader, "mouse_pick_shader");


    nk_spacing(ctx, 1);
    nk_labelf(ctx, NK_TEXT_LEFT, "mouse_x: %f", core_data->mouse_x);
    nk_labelf(ctx, NK_TEXT_LEFT, "mouse_y: %f", core_data->mouse_y);
    nk_labelf(ctx, NK_TEXT_LEFT, "mouse_delta_x: %f", core_data->mouse_delta_x);
    nk_labelf(ctx, NK_TEXT_LEFT, "mouse_delta_y: %f", core_data->mouse_delta_y);
    nk_labelf(ctx, NK_TEXT_LEFT, "scroll_x: %f", core_data->scroll_x);
    nk_labelf(ctx, NK_TEXT_LEFT, "scroll_y: %f", core_data->scroll_y);
    nk_labelf(ctx, NK_TEXT_LEFT, "scroll_delta_x: %f", core_data->scroll_delta_x);

    // more shaders
    nk_spacing(ctx, 1);
    gui_shader_properties(ctx, &core_data->equirect_shader, "equirect_shader");
    gui_shader_properties(ctx, &core_data->irradiance_map_shader, "irrandiance_map_shader");
    gui_shader_properties(ctx, &core_data->prefilter_shader, "prefilter_shader");
    
    nk_spacing(ctx, 1);
    gui_shader_properties(ctx, &core_data->terrain_shader, "terrain_shader");
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_materials_len: %d", core_data->terrain_materials_len);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_chunks_len: %d", core_data->terrain_chunks_len);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_scl: %f", core_data->terrain_scl);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_y_scl: %f", core_data->terrain_y_scl);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_x_len: %d", core_data->terrain_x_len);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_z_len: %d", core_data->terrain_z_len);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_layout_len: %d", core_data->terrain_layout_len);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_draw_dist: %d", core_data->terrain_draw_dist);
    nk_labelf(ctx, NK_TEXT_LEFT, "terrain_cull_dist: %d", core_data->terrain_cull_dist);
    
    nk_spacing(ctx, 1);
    nk_checkbox_label(ctx, "phys_act", &core_data->phys_act);
    nk_checkbox_label(ctx, "scripts_act", &core_data->scripts_act);

  }
  nk_end(ctx);
}

void gui_shader_properties(ui_context* ctx, shader_t* s, char* name)
{
  nk_labelf(ctx, NK_TEXT_LEFT, "%s:", name);
  nk_labelf(ctx, NK_TEXT_LEFT, "has_error: %s", STR_BOOL(s->has_error));
  nk_labelf(ctx, NK_TEXT_LEFT, "handle: %d", s->handle);
}
