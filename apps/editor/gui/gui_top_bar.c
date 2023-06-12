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
#include "editor/app.h"
#include "core/core_data.h"
#include "core/window.h"
#include "core/io/save_sys.h"

void gui_top_bar_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags)
{
  int w, h;
  window_get_size(&w, &h);

  core_data_t* core_data = core_data_get();;
  app_data_t*  app_data  = app_data_get();;
  
  float padding_x = ctx->style.window.padding.x;
  float padding_y = ctx->style.window.padding.y;
  ctx->style.window.padding.x = 0.0f;
  ctx->style.window.padding.y = 0.0f;

  // // less height because the window bar on top and below
  // top_bar_win_ratio.w = 1.0f;
  // top_bar_win_ratio.h = 0.045f; // pixel size harcoded
  // top_bar_win_ratio.x = 0.0f;;
  // top_bar_win_ratio.y = 0.0f; 

  // win_rect = nk_rect(top_bar_win_ratio.x * w, top_bar_win_ratio.y * h, 
  //                              top_bar_win_ratio.w * w, 35);// top_bar_win_ratio.h * h);
  if (nk_begin(ctx, "top bar", win_rect, win_flags)) 
  {
    // -- menubar --
    nk_menubar_begin(ctx);
    {
      ui_rect bounds; // used for tracking mouse hover for 'app_data->top_bar_menu_hover'
      nk_layout_row_begin(ctx, NK_STATIC, 30, 4);
      nk_layout_row_push(ctx, 50);
      if (nk_menu_begin_label(ctx, "scene", NK_TEXT_LEFT, nk_vec2(80, 200)))
      {
        nk_layout_row_dynamic(ctx, 20, 1);
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "save", NK_TEXT_LEFT))
        {
          save_sys_write_scene_to_file(SCENE_FILE_NAME); 
          save_sys_write_terrain_to_file(TERRAIN_FILE_NAME); 
        }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
       
        // @TODO: 
        if (nk_menu_item_label(ctx, "save as", NK_TEXT_LEFT))
        { }
        if (nk_menu_item_label(ctx, "import", NK_TEXT_LEFT))
        { }
        
        if (nk_menu_item_label(ctx, "new save", NK_TEXT_LEFT))
        {
          save_sys_write_empty_scene_to_file(); 
          // save_sys_write_empty_terrain_to_file(TERRAIN_FILE_NAME); 
        }
        
        nk_menu_end(ctx);
      }
      nk_layout_row_push(ctx, 55);
      if (nk_menu_begin_label(ctx, "windows", NK_TEXT_LEFT, nk_vec2(110, 200)))
      {
        nk_layout_row_static(ctx, 20, 90, 1);
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "hierarchy", NK_TEXT_LEFT))
        { app_data->show_hierarchy_win = !app_data->show_hierarchy_win; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "light hierarchy", NK_TEXT_LEFT))
        { app_data->show_light_hierarchy_win = !app_data->show_light_hierarchy_win; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "framebuffers", NK_TEXT_LEFT))
        { app_data->show_frameb_win = !app_data->show_frameb_win; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "debug", NK_TEXT_LEFT))
        { app_data->show_debug_win = !app_data->show_debug_win; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "core_data", NK_TEXT_LEFT))
        { app_data->show_core_data_win = !app_data->show_core_data_win; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        nk_menu_end(ctx);
      }
      nk_layout_row_push(ctx, 55);
      if (nk_menu_begin_label(ctx, "control", NK_TEXT_LEFT, nk_vec2(110, 200)))
      {
        nk_layout_row_static(ctx, 20, 90, 1);
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "play", NK_TEXT_LEFT))
        { core_data_play();  }// { core_data->phys_act = true; core_data->scripts_act = true; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
       
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "pause", NK_TEXT_LEFT))
        { core_data_pause(); } // { core_data->phys_act = false; core_data->scripts_act = false; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "phys play", NK_TEXT_LEFT)) { core_data_play_phys(); } // { core_data->phys_act = true; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        // bounds = nk_widget_bounds(ctx);
        // if (nk_menu_item_label(ctx, "phys pause", NK_TEXT_LEFT))  { core_data->phys_act = false; }
        // app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "script play", NK_TEXT_LEFT)) { core_data_play_scripts(); } // { core_data->scripts_act = true; }
        app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        
        // bounds = nk_widget_bounds(ctx);
        // if (nk_menu_item_label(ctx, "script pause", NK_TEXT_LEFT))  { core_data->scripts_act = false; }
        // app_data->top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : app_data->top_bar_menu_hover;
        nk_menu_end(ctx);
      
      }
        nk_layout_row_push(ctx, 30);
        if (nk_button_symbol(ctx, !core_data_is_play() ? NK_SYMBOL_TRIANGLE_RIGHT : NK_SYMBOL_X))
        {
          if (!core_data_is_play()) { core_data_play(); }
          else { core_data_pause(); }
        }
      
      if (app_data->gui_info_t >= 0.0f)
      {
        nk_layout_row_push(ctx, 250);
        nk_labelf(ctx, NK_TEXT_LEFT, " | %s", app_data->gui_info_str);
        
        app_data->gui_info_t -= core_data->delta_t;
      }
    }
    nk_menubar_end(ctx);
  }
  nk_end(ctx); 
  
  // reset state
  ctx->style.window.padding.x = padding_x;
  ctx->style.window.padding.y = padding_y;
}
