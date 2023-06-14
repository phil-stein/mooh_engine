#include "editor/gui/gui.h"
#include "editor/gui/gui_style.h"
#include "editor/app.h"
#include "editor/gizmo.h"
#include "editor/operation.h"
#include "editor/stylesheet.h"
#include "core/core_data.h"
#include "core/window.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"
#include "core/renderer/renderer_extra.h"
#include "core/state.h"
#include "core/input.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/camera.h"
#include "core/types/types.h"
#include "core/debug/debug_timer.h"
#include "core/debug/debug_draw.h"
#include "data/entity_template.h"

// @NOTE: tmp
#include "phys/phys_world.h"
#include "phys/phys_debug_draw.h"

#include "stb/stb_ds.h"

#include "GLAD/glad.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


// ---- vars ----
int window_w;
int window_h;

ui_color red = { 255, 0, 0, 255 };

ui_glfw glfw = { 0 };
int width = 0, height = 0;
ui_context* ctx;
ui_colorf bg;

// cant have these two because the windoews cant be resized otherwise NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE
const nk_flags window_flags       = NK_WINDOW_BORDER | NK_WINDOW_TITLE;
const nk_flags window_min_flags   = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE;
const nk_flags window_float_flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE;
const nk_flags top_bar_flags      = NK_WINDOW_BORDER;

ui_rect top_bar_win_rect;
ui_rect top_bar_win_ratio;
ui_rect prop_win_rect;
ui_rect prop_win_ratio;
ui_rect template_win_rect;
ui_rect template_win_ratio;
ui_rect struct_browser_win_rect;
ui_rect struct_browser_win_ratio;
ui_rect hierarchy_win_rect;
ui_rect hierarchy_win_ratio;
ui_rect frameb_win_rect;
ui_rect debug_win_rect;
ui_rect light_hierarchy_win_rect;
ui_rect light_hierarchy_win_ratio;
ui_rect core_data_win_rect;


static core_data_t* core_data = NULL;
static app_data_t*  app_data  = NULL;

void gui_init()
{
  core_data = core_data_get();
  app_data  = app_data_get();
  
  // ---- nuklear ----
  ctx = nk_glfw3_init(&glfw, core_data->window, NK_GLFW3_INSTALL_CALLBACKS);
  /* Load Fonts: if none of these are loaded a default font will be used  */
  /* Load Cursor: if you uncomment cursor loading please hide the cursor */
  {struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);

    struct nk_font *droid = nk_font_atlas_add_from_file(atlas, ASSET_PATH"fonts/DroidSans.ttf", 20/*18*//*16*//*14*/, 0);

    nk_glfw3_font_stash_end(&glfw);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

    nk_style_set_font(ctx, &droid->handle);
  }

  app_data->fb_preview.type = FRAMEBUFFER_RGB;
  app_data->fb_preview.is_msaa = false;
  app_data->fb_preview.width  = 512;
  app_data->fb_preview.height = 512;
  app_data->fb_preview.size_divisor = 1;
  framebuffer_create(&app_data->fb_preview);

  // P_NK_COLOR(ctx->style.button.text_active);
  // P_NK_COLOR(ctx->style.button.text_hover); 
  // P_NK_COLOR(ctx->style.button.text_normal);
}

void gui_update()
{ 
  // get window size
  window_get_size(&window_w, &window_h);
  int w = window_w; int h = window_h;
  nk_glfw3_new_frame(&glfw);
  
  // windows ------------------------------------------------------------------------------------------

  if (!core_data_is_play())
  {
    // less height because the window bar on top and below
    top_bar_win_ratio.w = 1.0f;
    top_bar_win_ratio.h = 0.045f; // pixel size harcoded
    top_bar_win_ratio.x = 0.0f;;
    top_bar_win_ratio.y = 0.0f; 

    top_bar_win_rect = nk_rect(top_bar_win_ratio.x * w, top_bar_win_ratio.y * h, 
                               top_bar_win_ratio.w * w, 45);// top_bar_win_ratio.h * h);
    gui_top_bar_win(ctx, top_bar_win_rect, top_bar_flags);

    // less height because the window bar on top and below
    template_win_ratio.w = 1.0f - prop_win_ratio.w; 
    template_win_ratio.h = 0.2f; 
    template_win_ratio.x = 0.0f; 
    template_win_ratio.y = app_data->template_browser_minimized ? 1.0f : 0.8f;

    template_win_rect = nk_rect(template_win_ratio.x * w, template_win_ratio.y * h, 
                                template_win_ratio.w * w, template_win_ratio.h * h);
    gui_template_browser_win(ctx, template_win_rect, window_min_flags);
  }
  // less height because the window bar on top and below
  prop_win_ratio.w = 0.25f; 
  prop_win_ratio.h = 1.0f;  
  prop_win_ratio.x = 0.75f; 
  prop_win_ratio.y = 0.0f;  

  bool is_selected = app_data->selected_id >= 0 || 
                     app_data->selected_id <= ID_BUFFER_TERRAIN_0; // -2: terrain
  prop_win_ratio.w *= is_selected ? 1.0f : 0.0f;

  prop_win_rect = nk_rect(prop_win_ratio.x * w, prop_win_ratio.y * h + top_bar_win_rect.h,
                          prop_win_ratio.w * w, prop_win_ratio.h * h);
  gui_properties_win(ctx, prop_win_rect, window_flags, is_selected);
 
  // --- external ---
  

  if (!core_data_is_play())
  { 
    struct_browser_win_ratio.h = 1.0f;
    struct_browser_win_ratio.w = 0.1f;
    struct_browser_win_ratio.x = 0.0f;
    struct_browser_win_ratio.y = 0.0f;
    int h_correct = top_bar_win_rect.h + (template_win_rect.h * !app_data->template_browser_minimized) + (35 * app_data->template_browser_minimized);
    struct_browser_win_rect = nk_rect((struct_browser_win_ratio.x * w), 
                                      (struct_browser_win_ratio.y * h) + top_bar_win_rect.h, 
                                      (struct_browser_win_ratio.w * w), 
                                      (struct_browser_win_ratio.h * h) - h_correct);
    gui_struct_browser_win(ctx, struct_browser_win_rect, window_min_flags); 
  }

  // --- optional ---

  if (app_data->show_hierarchy_win)
  { gui_hierarchy_win(); }
  
  if (app_data->show_light_hierarchy_win)
  { gui_light_hierarchy_win(); }

  if (app_data->show_frameb_win)
  { gui_framebuffer_win(); }

   if (app_data->show_debug_win)
  { gui_debug_win(); } 

   if (app_data->show_core_data_win)
  {
    // less height because the window bar on top and below
    const f32 w_ratio = 400.0f  / 1920.0f;
    const f32 h_ratio = 1000.0f / 1020.0f;
    const f32 x_ratio = 0.0f    / 1920.0f;
    const f32 y_ratio = 10.0f   / 1020.0f + top_bar_win_ratio.h;

    core_data_win_rect = nk_rect(x_ratio * w, y_ratio * h, w_ratio * w, h_ratio * h);
    gui_core_data_win(ctx, core_data_win_rect, window_float_flags); 
  } 
 
   // --------------------------------------------------------------------------------------------------

  nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

  // mouse over gui -----------------------------------------------------------------------------------

  bool over_ui = false;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, top_bar_win_rect)        ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, prop_win_rect)           ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, template_win_rect)       ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, struct_browser_win_rect) ? true : over_ui;
  
  if (app_data->show_hierarchy_win)       { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, hierarchy_win_rect)       ? true : over_ui; }
  if (app_data->show_light_hierarchy_win) { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, light_hierarchy_win_rect) ? true : over_ui; }
  if (app_data->show_frameb_win)          { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, frameb_win_rect)          ? true : over_ui; }
  if (app_data->show_debug_win)           { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, debug_win_rect)           ? true : over_ui; }
  if (app_data->show_core_data_win)       { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, core_data_win_rect)       ? true : over_ui; }

  // already hovering or hovewr check from top bar menu's
  over_ui = over_ui || app_data->top_bar_menu_hover;  
  app_data->top_bar_menu_hover = false;

  app_data->mouse_over_ui = over_ui;
}

void gui_cleanup()
{
  nk_clear(ctx);
}


void gui_hierarchy_win()
{
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  hierarchy_win_ratio.w = 0.1f;
  hierarchy_win_ratio.h = 1.0f - template_win_ratio.h;
  hierarchy_win_ratio.x = 0.0f;
  hierarchy_win_ratio.y = 0.0f + top_bar_win_ratio.h; 

  hierarchy_win_rect = nk_rect(hierarchy_win_ratio.x * w, hierarchy_win_ratio.y * h, 
                               hierarchy_win_ratio.w * w, hierarchy_win_ratio.h * h);
  if (nk_begin(ctx, "entity hierarchy", hierarchy_win_rect, window_float_flags)) 
  {
    nk_layout_row_dynamic(ctx, 30, 1);
    int e_len = 0;
    int e_dead_len = 0;
    entity_t* e = state_entity_get_arr(&e_len, &e_dead_len);
    // int selected_id = app_get_selected_id();
    for (int i = 0; i < e_len; ++i)
    {
      int offs = 0;
      if (e[i].parent <= -1)
      { gui_hierarchy_display_entity_and_children(&e[i], &offs); }
    }
  }
  nk_end(ctx); 
}
void gui_hierarchy_display_entity_and_children(entity_t* e, int* offs)
{
  // bool err = false;
  // entity_t* e = state_entity_get(id, &err);
  // assert(err);
  if (!e->is_dead)
  {
    nk_layout_row_begin(ctx, NK_STATIC, 30, *offs == 0 ? 1 : 2);
    
    const float offs_step = 10.0f;
    if (*offs > 0)
    { nk_layout_row_push(ctx, *offs * offs_step); nk_spacing(ctx, 1); } 
    
    nk_layout_row_push(ctx, hierarchy_win_rect.w - (*offs * offs_step));
    char buf[32];
    sprintf(buf, "%d", e->id);
    bool selec = e->id == app_data->selected_id; 
    bool selec_old = selec;
    nk_selectable_label(ctx, buf, NK_TEXT_LEFT, &selec);
    if (!selec_old && selec)
    { app_data->selected_id = e->id; }

    *offs += 1;
    // PF("offs: %d, id: %d\n", *offs, e->id);
    for (int i = 0; i < e->children_len; ++i)
    {
      ERR_CHECK(e->id != e->children[i], "id: %d, children_len: %d, child[%d]: %d\n", e->id, e->children_len, i, e->children[i]);
      entity_t* c = state_entity_get(e->children[i]);
      gui_hierarchy_display_entity_and_children(c, offs); 
    }
  }   
}

void gui_light_hierarchy_win()
{
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  light_hierarchy_win_ratio.w = 0.1f;
  light_hierarchy_win_ratio.h = 1.0f - template_win_ratio.h;
  light_hierarchy_win_ratio.x = 0.0f;
  light_hierarchy_win_ratio.y = 0.0f + top_bar_win_ratio.h; 

  light_hierarchy_win_rect = nk_rect(light_hierarchy_win_ratio.x * w, light_hierarchy_win_ratio.y * h, 
                                     light_hierarchy_win_ratio.w * w, light_hierarchy_win_ratio.h * h);
  if (nk_begin(ctx, "light hierarchy", light_hierarchy_win_rect, window_float_flags)) 
  {
    nk_layout_row_dynamic(ctx, 30, 1);
    int dl_len = 0;
    dir_light_t* dl   = state_dir_light_get_arr(&dl_len);
    int pl_len = 0;
    int pl_dead_len = 0;
    point_light_t* pl = state_point_light_get_arr(&pl_len, &pl_dead_len);

    const int SEL_LIGHT_NONE  = 0;
    const int SEL_LIGHT_DIR   = 1;
    const int SEL_LIGHT_POINT = 2;

    static int selected_type = 0; // vs is stupid so commented out SEL_LIGHT_NONE;
    static int selected = -1;


    nk_property_float(ctx, "cube map intensity", 0.0f, &core_data->cube_map.intensity, 100.0f, 0.1f, 0.01f);

    if (nk_button_label(ctx, "add dir light")) 
    {
      state_dir_light_add(VEC3(0), VEC3_Y(-1), RGB_F(1, 1, 1), 1, false, 0, 0);
    } 
    if (nk_button_label(ctx, "add point light"))
    {
      vec3 front, pos;
      vec3_copy(core_data->cam.front, front); // camera_get_front(front);
      vec3_copy(core_data->cam.pos,   pos);   // camera_get_pos(pos);
      vec3_mul_f(front, 8.0f, front);
      vec3_add(front, pos, pos);
      state_point_light_add_empty(pos, RGB_F(1.0f, 0.0f, 1.0f), 1.0f);
    }

    if (nk_tree_push(ctx, NK_TREE_TAB, "hierarchy", NK_MAXIMIZED))
    {
      for (int i = 0; i < dl_len; ++i)
      {
        char buf[32];
        sprintf(buf, "dir light: %d", i);
        bool selec = i == selected && selected_type == SEL_LIGHT_DIR;
        nk_selectable_label(ctx, buf, NK_TEXT_LEFT, &selec);
        if (selec) { selected = i; selected_type = SEL_LIGHT_DIR; }     // select
        if (!selec && i == selected && selected_type == SEL_LIGHT_DIR) 
        { selected = -1; selected_type = SEL_LIGHT_NONE; }              // deselect
      }
      for (int i = 0; i < pl_len; ++i)
      {
        if (pl->is_dead) { continue; }
        char buf[32];
        sprintf(buf, "point light: %d", i);
        bool selec = i == selected && selected_type == SEL_LIGHT_POINT;
        nk_selectable_label(ctx, buf, NK_TEXT_LEFT, &selec);
        // if (selec) { selected = i; selected_type = SEL_LIGHT_POINT; debug_draw_sphere_register(pl[i].pos, 0.35f, pl[i].color); } // select
        if (!selec && i == selected && selected_type == SEL_LIGHT_POINT) 
        { selected = -1; selected_type = SEL_LIGHT_NONE; }                                                                       // deselect
      }
      nk_tree_pop(ctx);
    }

    if (selected >= 0 && selected_type == SEL_LIGHT_DIR && nk_tree_push(ctx, NK_TREE_TAB, "properties", NK_MAXIMIZED))
    {
      dir_light_t* l = &dl[selected];
      nk_layout_row_dynamic(ctx, 30, 1);

      if (nk_button_label(ctx, "remove")) 
      {
        state_dir_light_remove(selected);
        selected = -1;
      }
      
      nk_label(ctx, "direction", NK_TEXT_LEFT);
      nk_property_float(ctx, "d.x", -2048.0f, &l->dir[0], 2048.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "d.y", -2048.0f, &l->dir[1], 2048.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "d.z", -2048.0f, &l->dir[2], 2048.0f, 0.1f, 0.01f);
      
      gui_color_selector(l->color);
      
      nk_property_float(ctx, "intens.", -1.0f, &l->intensity, 2048.0f, 0.1f, 0.01f);

      if (selected == 0)
      {
        // bool tmp = &l->cast_shadow;
        // l->cast_shadow = nk_checkbox_label(ctx, "cast shadows", &tmp);
        nk_checkbox_label(ctx, "cast shadows", &l->cast_shadow);

        nk_labelf(ctx, NK_TEXT_LEFT, "shadowmap");
        nk_labelf(ctx, NK_TEXT_LEFT, "x: %d, y: %d", l->shadow_map_x, l->shadow_map_y);
      }
      
      nk_tree_pop(ctx);
    }
    else if (selected >= 0 && selected_type == SEL_LIGHT_POINT && nk_tree_push(ctx, NK_TREE_TAB, "properties", NK_MAXIMIZED))
    {
      point_light_t* l = &pl[selected];
      nk_layout_row_dynamic(ctx, 30, 1);

      if (nk_button_label(ctx, "remove")) 
      {
        state_point_light_remove(selected);
        selected = -1;
      }

      nk_label(ctx, "position offset", NK_TEXT_LEFT);
      nk_property_float(ctx, "p.x", -2048.0f, &l->offset[0], 2048.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "p.y", -2048.0f, &l->offset[1], 2048.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "p.z", -2048.0f, &l->offset[2], 2048.0f, 0.1f, 0.01f);
      // nk_labelf(ctx, NK_LEFT, "p.x %.2f", (*l->pos_ptr)[0]);
      // nk_labelf(ctx, NK_LEFT, "p.y %.2f", (*l->pos_ptr)[1]);
      // nk_labelf(ctx, NK_LEFT, "p.z %.2f", (*l->pos_ptr)[2]);
      
      gui_color_selector(l->color);
      
      nk_property_float(ctx, "intens.", -1.0f, &l->intensity, 2048.0f, 0.1f, 0.01f);
      
      nk_tree_pop(ctx);
    }
  }
  nk_end(ctx); 
}

void gui_framebuffer_win()
{
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  const f32 w_ratio = 400.0f  / 1920.0f;
  const f32 h_ratio = 1000.0f / 1020.0f;
  const f32 x_ratio = 0.0f    / 1920.0f;
  const f32 y_ratio = 10.0f   / 1020.0f + top_bar_win_ratio.h;

  frameb_win_rect = nk_rect(x_ratio * w, y_ratio * h, w_ratio * w, h_ratio * h);
  if (nk_begin(ctx, "framebuffers", frameb_win_rect, window_float_flags)) 
  {
    core_data_t* core_data = core_data_get();

    int w, h;
    window_get_size(&w, &h);
    float ratio = (float)h / (float)w;
    // const int size = 150; // 190; // 245;
    int size = frameb_win_rect.w - 60;
    
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "deferred - color");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_deferred.buffer));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "deferred - material");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_deferred.buffer02));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "deferred - normal");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_deferred.buffer03));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "deferred - position");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_deferred.buffer04));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "shadow");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_shadow_pass.buffer));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "lighting");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_lighting.buffer));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "mouse_pick");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_mouse_pick.buffer));
    nk_layout_row_static(ctx, 25, size, 1);
    nk_labelf(ctx, NK_LEFT, "outline");
    nk_layout_row_static(ctx, size * ratio, size, 1);
    nk_image(ctx, nk_image_id(core_data->fb_outline.buffer));
  }
  nk_end(ctx);
}
void gui_debug_win()
{
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  const f32 w_ratio = 400.0f  / 1920.0f;
  const f32 h_ratio = 1000.0f / 1020.0f;
  const f32 x_ratio = 0.0f    / 1920.0f;
  const f32 y_ratio = 10.0f   / 1020.0f + top_bar_win_ratio.h;

  debug_win_rect = nk_rect(x_ratio * w, y_ratio * h, w_ratio * w, h_ratio * h);
  if (nk_begin(ctx, "debug", debug_win_rect, window_float_flags)) 
  {
    
    nk_layout_row_dynamic(ctx, 25, 1);
    nk_labelf(ctx, NK_TEXT_LEFT, "draw_calls_total:         %9u", core_data->draw_calls_total);
    nk_labelf(ctx, NK_TEXT_LEFT, "  draw_calls_shadow:      %6u", core_data->draw_calls_shadow);
    nk_labelf(ctx, NK_TEXT_LEFT, "  draw_calls_deferred:    %6u", core_data->draw_calls_deferred);
    nk_labelf(ctx, NK_TEXT_LEFT, "  draw_calls_screen_quad: %2u", core_data->draw_calls_screen_quad);
    nk_labelf(ctx, NK_TEXT_LEFT, "  draw_call for cubemap:    1");

    
    int idxs_len = 0;
    int** idxs = state_entity_get_template_idxs_arr(&idxs_len);
    for (u32 t = 0; t < idxs_len; ++t)
    {
      nk_labelf(ctx, NK_TEXT_LEFT, "template: %d", t);
      
      u32 len = arrlen(idxs[t]);
      for (u32 i = 0; i < len; ++i)
      {
        nk_labelf(ctx, NK_TEXT_LEFT, "  -> %d", idxs[t][i]);
      }
    }

    // nk_spacing(ctx, 1);

    #ifdef DEBUG_TIMER
    if (nk_tree_push(ctx, NK_TREE_TAB, "static timers", NK_MINIMIZED))
    {
      char buf[128];
      int len = 0;
      timer_t* timers = debug_timer_get_all_static(&len);

      nk_layout_row_dynamic(ctx, 25, 1);
      if (len > 0) // lable for first file name
      {
        sprintf(buf, "[%s]", timers[0].file);
        nk_label(ctx, buf, NK_TEXT_LEFT);
      }
      for (int i = 0; i < len; ++i)
      {
        // @UNCLEAR: im comparing pointers here and it works 
        //           presumably because __FILE__, __FUNCTION__ always points to the same string
        //           not sure if i should use strcmp() anyways, if its more stable yk
        if (i != 0 && (timers[i - 1].file != timers[i].file)) // print file name if it changed
        {
          sprintf(buf, "[%s]", timers[i].file);
          nk_label(ctx, buf, NK_TEXT_LEFT);
        }
        sprintf(buf, "-> | %.2fms | %s | line: %d", timers[i].time, timers[i].name, timers[i].line);
        nk_label(ctx, buf, NK_TEXT_LEFT);
      }
      sprintf(buf, "[frame] -> %.2fms", core_data->delta_t * 1000); // * 1000: concvert to ms
      nk_label(ctx, buf, NK_TEXT_LEFT);
      nk_tree_pop(ctx);
    }
    if (nk_tree_push(ctx, NK_TREE_TAB, "timers", NK_MINIMIZED))
    {
      char buf[128];
      int len = 0;
      timer_t* timers = debug_timer_get_all(&len);

      nk_layout_row_dynamic(ctx, 25, 1);
      if (len > 0) // lable for first file name
      {
        sprintf(buf, "[%s]", timers[0].file);
        nk_label(ctx, buf, NK_TEXT_LEFT);
      }
      for (int i = 0; i < len; ++i)
      {
        // @UNCLEAR: im comparing pointers here and it works 
        //           presumably because __FILE__, __FUNCTION__ always points to the same string
        //           not sure if i should use strcmp() anyways, if its more stable yk
        if (i != 0 && (timers[i - 1].file != timers[i].file)) // print file name if it changed
        {
          sprintf(buf, "[%s]", timers[i].file);
          nk_label(ctx, buf, NK_TEXT_LEFT);
        }
        sprintf(buf, "-> | %.2fms | %s | line: %d", timers[i].time, timers[i].name, timers[i].line);
        nk_label(ctx, buf, NK_TEXT_LEFT);
      }
      sprintf(buf, "[frame] -> %.2fms", core_data->delta_t * 1000); // * 1000: concvert to ms
      nk_label(ctx, buf, NK_TEXT_LEFT);
      nk_tree_pop(ctx);
    }
    #else
    nk_layout_row_dynamic(ctx, 25, 1);
    nk_label(ctx, "timers deactivated", NK_TEXT_LEFT);
    #endif
  }
  nk_end(ctx);
}


// -- gui elems --

void gui_color_selector(rgbf color)
{ 
  ui_colorf c = { color[0], color[1], color[2] };

  // complex color combobox
  if (nk_combo_begin_color(ctx, nk_rgb_cf(c), nk_vec2(200, 400)))
  {
    enum color_mode { COL_RGB, COL_HSV };
    static int col_mode = COL_RGB;

    nk_layout_row_dynamic(ctx, 120, 1);
    c = nk_color_picker(ctx, c, NK_RGB);

    nk_layout_row_dynamic(ctx, 25, 2);
    col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
    col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

    nk_layout_row_dynamic(ctx, 25, 1);
    if (col_mode == COL_RGB) {
      c.r = nk_propertyf(ctx, "#R:", 0, c.r, 1.0f, 0.01f, 0.005f);
      c.g = nk_propertyf(ctx, "#G:", 0, c.g, 1.0f, 0.01f, 0.005f);
      c.b = nk_propertyf(ctx, "#B:", 0, c.b, 1.0f, 0.01f, 0.005f);
    }
    else {
      float hsva[4];
      nk_colorf_hsva_fv(hsva, c);
      hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
      hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
      hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
      c = nk_hsva_colorfv(hsva);
    }
    nk_combo_end(ctx);
    
    color[0] = c.r;
    color[1] = c.g;
    color[2] = c.b;

  }
}

