#include "editor/gui/gui.h"
#include "editor/gui/gui_style.h"
#include "editor/app.h"
#include "editor/gizmo.h"
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

framebuffer_t fb_preview;

bool top_bar_menu_hover         = false;   // mouse over menu in top bar
bool template_browser_minimized = true;

bool show_hierarchy_win       = false;
bool show_frameb_win          = false;
bool show_debug_win           = false;
bool show_light_hierarchy_win = false;
bool show_core_data_win       = false;

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

    struct nk_font *droid = nk_font_atlas_add_from_file(atlas, ASSET_PATH"fonts/DroidSans.ttf", 18/*16*//*14*/, 0);

    nk_glfw3_font_stash_end(&glfw);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

    nk_style_set_font(ctx, &droid->handle);
  }

  fb_preview.type = FRAMEBUFFER_RGB;
  fb_preview.is_msaa = false;
  fb_preview.width  = 512;
  fb_preview.height = 512;
  fb_preview.size_divisor = 1;
  framebuffer_create(&fb_preview);

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

  gui_top_bar_win();
  gui_properties_win();
  gui_template_browser_win();
 
  // --- external ---
  
  struct_browser_win_ratio.h = 1.0f;
  struct_browser_win_ratio.w = 0.1f;
  struct_browser_win_ratio.x = 0.0f;
  struct_browser_win_ratio.y = 0.0f;
  int h_correct = top_bar_win_rect.h + (template_win_rect.h * !template_browser_minimized) + (35 * template_browser_minimized);
  struct_browser_win_rect = nk_rect((struct_browser_win_ratio.x * w), 
                                    (struct_browser_win_ratio.y * h) + top_bar_win_rect.h, 
                                    (struct_browser_win_ratio.w * w), 
                                    (struct_browser_win_ratio.h * h) - h_correct);
  gui_struct_browser_win(ctx, struct_browser_win_rect, window_min_flags);

  // --- optional ---

  if (show_hierarchy_win)
  { gui_hierarchy_win(); }
  
  if (show_light_hierarchy_win)
  { gui_light_hierarchy_win(); }

  if (show_frameb_win)
  { gui_framebuffer_win(); }

   if (show_debug_win)
  { gui_debug_win(); } 

   if (show_core_data_win)
  { gui_core_data_win(); } 
 
   // --------------------------------------------------------------------------------------------------

  nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

  // mouse over gui -----------------------------------------------------------------------------------

  bool over_ui = false;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, top_bar_win_rect)        ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, prop_win_rect)           ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, template_win_rect)       ? true : over_ui;
  over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, struct_browser_win_rect) ? true : over_ui;
  
  if (show_hierarchy_win)       { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, hierarchy_win_rect)       ? true : over_ui; }
  if (show_light_hierarchy_win) { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, light_hierarchy_win_rect) ? true : over_ui; }
  if (show_frameb_win)          { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, frameb_win_rect)          ? true : over_ui; }
  if (show_debug_win)           { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, debug_win_rect)           ? true : over_ui; }
  if (show_core_data_win)       { over_ui = nk_input_is_mouse_hovering_rect(&ctx->input, core_data_win_rect)       ? true : over_ui; }

  // already hovering or hovewr check from top bar menu's
  over_ui = over_ui || top_bar_menu_hover;  
  top_bar_menu_hover = false;

  app_data->mouse_over_ui = over_ui;
}

void gui_cleanup()
{
  nk_clear(ctx);
}

void gui_top_bar_win()
{
  int w, h;
  window_get_size(&w, &h);

  float padding_x = ctx->style.window.padding.x;
  float padding_y = ctx->style.window.padding.y;
  ctx->style.window.padding.x = 0.0f;
  ctx->style.window.padding.y = 0.0f;

  // less height because the window bar on top and below
  top_bar_win_ratio.w = 1.0f;
  top_bar_win_ratio.h = 0.045f; // pixel size harcoded
  top_bar_win_ratio.x = 0.0f;;
  top_bar_win_ratio.y = 0.0f; 

  top_bar_win_rect = nk_rect(top_bar_win_ratio.x * w, top_bar_win_ratio.y * h, 
                               top_bar_win_ratio.w * w, 35);// top_bar_win_ratio.h * h);
  if (nk_begin(ctx, "top bar", top_bar_win_rect, top_bar_flags)) 
  {
    // -- menubar --
    nk_menubar_begin(ctx);
    {
      ui_rect bounds; // used for tracking mouse hover for 'top_bar_menu_hover'
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
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
       
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
        { show_hierarchy_win = !show_hierarchy_win; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "light hierarchy", NK_TEXT_LEFT))
        { show_light_hierarchy_win = !show_light_hierarchy_win; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "framebuffers", NK_TEXT_LEFT))
        { show_frameb_win = !show_frameb_win; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "debug", NK_TEXT_LEFT))
        { show_debug_win = !show_debug_win; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "core_data", NK_TEXT_LEFT))
        { show_core_data_win = !show_core_data_win; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        nk_menu_end(ctx);
      }
      nk_layout_row_push(ctx, 55);
      if (nk_menu_begin_label(ctx, "control", NK_TEXT_LEFT, nk_vec2(110, 200)))
      {
        nk_layout_row_static(ctx, 20, 90, 1);
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "play", NK_TEXT_LEFT))
        { core_data_play();  }// { core_data->phys_act = true; core_data->scripts_act = true; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
       
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "pause", NK_TEXT_LEFT))
        { core_data_pause(); } // { core_data->phys_act = false; core_data->scripts_act = false; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "phys play", NK_TEXT_LEFT)) { core_data_play_phys(); } // { core_data->phys_act = true; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        // bounds = nk_widget_bounds(ctx);
        // if (nk_menu_item_label(ctx, "phys pause", NK_TEXT_LEFT))  { core_data->phys_act = false; }
        // top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        bounds = nk_widget_bounds(ctx);
        if (nk_menu_item_label(ctx, "script play", NK_TEXT_LEFT)) { core_data_play_scripts(); } // { core_data->scripts_act = true; }
        top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        // bounds = nk_widget_bounds(ctx);
        // if (nk_menu_item_label(ctx, "script pause", NK_TEXT_LEFT))  { core_data->scripts_act = false; }
        // top_bar_menu_hover = nk_input_is_mouse_hovering_rect(&ctx->input, bounds) ? true : top_bar_menu_hover;
        
        nk_menu_end(ctx);
      }
    }
    nk_menubar_end(ctx);
  }
  nk_end(ctx); 
  
  // reset state
  ctx->style.window.padding.x = padding_x;
  ctx->style.window.padding.y = padding_y;
}
void gui_properties_win()
{
  // window test --------------------------------------------------------------------------------------
  int w, h;
  window_get_size(&w, &h);

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
  if (is_selected && nk_begin(ctx, "properties", prop_win_rect, window_flags)) 
  {
    // -- properties --

    int id = app_data->selected_id;
    if (id >= 0)  // entities
    {
      entity_t* e     = state_entity_get(id);
      int world_len = 0; int dead_len = 0;
      state_entity_get_arr(&world_len, &dead_len);
      
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_labelf(ctx, NK_TEXT_LEFT, "id: %d, table_idx: %d", id, e->template_idx);
      nk_labelf(ctx, NK_TEXT_LEFT, "parent: %d, #children: %d", e->parent, e->children_len);
      nk_layout_row_dynamic(ctx, 25, 2);
      static int parent_id = 0;
      nk_property_int(ctx, "parent id", 0, &parent_id, world_len -1, 1, 0.1f);
      if (nk_button_label(ctx, "set parent") && parent_id >= 0 && parent_id < world_len)
      {
        bool error = false;
        state_entity_get_err(parent_id, &error);  // just checking if exists // @TODO: isnt there a state func for that
        if (!error)
        {
          state_entity_add_child_remove_parent(parent_id, id);
        }
        else { ERR("parent id not valid: %d", parent_id); }
      }

      nk_layout_row_dynamic(ctx, 25, 1);
      if (nk_button_label(ctx, "remove parent") && e->parent >= 0)
      {
        state_entity_remove_child(e->parent, e->id);
      }

      // @TODO: @UNSURE: display children

      nk_layout_row_dynamic(ctx, 25, 2);
      nk_labelf(ctx, NK_TEXT_LEFT, "init:   %s", STR_BOOL(e->init_f != NULL));
      nk_labelf(ctx, NK_TEXT_LEFT, "update: %s", STR_BOOL(e->update_f != NULL));
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_labelf(ctx, NK_TEXT_LEFT, "is_moved: %s", STR_BOOL(e->is_moved));
      

      nk_layout_row_dynamic(ctx, 30, 2);
      if (nk_button_label(ctx, "remove"))
      {
        state_entity_remove(app_data->selected_id);
        app_data->selected_id = -1;
      }
      if (nk_button_label(ctx, "duplicate"))
      {
        int id = state_entity_duplicate(app_data->selected_id, VEC3_XYZ(2, 0, 0));
        app_data->selected_id = id;
      }
      nk_layout_row_dynamic(ctx, 30, 1);

      nk_layout_row_dynamic(ctx, 30, 2);
      static char struct_name_buffer[64] = "\0";
      nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD | NK_EDIT_AUTO_SELECT, struct_name_buffer, sizeof(struct_name_buffer), nk_filter_ascii);
      bool btn_act = strlen(struct_name_buffer) > 0;
      gui_style_set_button_color(ctx, btn_act);
      if (nk_button_symbol_label(ctx, btn_act ? NK_SYMBOL_NONE : NK_SYMBOL_X, "save as structure", NK_TEXT_ALIGN_RIGHT))
      {
        save_sys_write_structure_to_file(struct_name_buffer, e->id);
      }
      gui_style_set_button_color(ctx, true);
      
      nk_layout_row_dynamic(ctx, 30, 1);

      // -- components --

      if (nk_tree_push(ctx, NK_TREE_TAB, "transform", NK_MINIMIZED))
      {
        gui_properties_transform(e->pos, e->rot, e->scl, &e->is_moved);
        nk_tree_pop(ctx);
      }
      
      if (e->mat >= 0 && e->mesh >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "material", NK_MINIMIZED))
      {
        material_t* mat = assetm_get_material_by_idx(e->mat);
        gui_properties_material(mat, e->mat);
        nk_tree_pop(ctx);
      }
      
      if (e->mesh >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "mesh", NK_MINIMIZED))
      {
        mesh_t* m = assetm_get_mesh_by_idx(e->mesh);
        gui_properties_mesh(m, e->mesh, e->template_idx);
        nk_tree_pop(ctx);
      }

      if (e->point_light_idx >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "point light", NK_MINIMIZED))
      {
        bool error = false;
        point_light_t* p = state_point_light_get(e->point_light_idx, &error);
        // gui_properties_point_light(e->pos, VEC3(1), 1);
        gui_properties_point_light(p, e->point_light_idx);
        nk_tree_pop(ctx);
      }
      else 
      {
        if (nk_button_label(ctx, "add pointlight"))
        { state_point_light_add(VEC3(0), VEC3(1), 1.0f, e->id); }
      }

      const entity_template_t* def = entity_template_get(e->template_idx);
      if (def->phys_flag != 0 && nk_tree_push(ctx, NK_TREE_TAB, "physics", NK_MINIMIZED))
      {
        gui_properties_physics(def, e);
        nk_tree_pop(ctx);
      }

    } 
    else if (id <= ID_BUFFER_TERRAIN_0)  // terrain
    {
      int idx = ID_BUFFER_TERRAIN_TO_CHUNK_IDX(id);
      nk_labelf(ctx, NK_TEXT_LEFT, "[terrain]");
      nk_labelf(ctx, NK_TEXT_LEFT, "-> id: %d, idx: %d", id, idx);

      terrain_chunk_t* chunk = &core_data->terrain_chunks[idx];
      
      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "edit", NK_MINIMIZED))
      {
        // nk_selectable_label(ctx, "edit", NK_TEXT_CENTERED, &app_data->terrain_edit_act); 
       
        const char* names[]  = { "none", "sculpt", "level", "smooth", "paint" };
        const int names_len = 5;
        app_data->terrain_edit_type = nk_combo(ctx, names, names_len, app_data->terrain_edit_type, 25, nk_vec2(200, 200));

        nk_property_float(ctx, "strength", 0.0f, &app_data->terrain_edit_strength, 10.0f, 0.1f, 0.01f);
        nk_property_float(ctx, "radius", 0.0f, &app_data->terrain_edit_radius, 20.0f, 0.1f, 0.01f);
        if (app_data->terrain_edit_type == TERRAIN_EDIT_PAINT)
        {
          nk_property_int(ctx, "material", 0, &app_data->terrain_edit_paint_material, core_data->terrain_materials_len, 1, 0.01f);
        }
        nk_tree_pop(ctx);
      }

      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "chunk", NK_MINIMIZED))
      {
        nk_labelf(ctx, NK_TEXT_LEFT, "pos [ %.2f, %.2f, %.2f ]", chunk->pos[0], chunk->pos[1], chunk->pos[2]);
        nk_labelf(ctx, NK_TEXT_LEFT, "strips_num: %d, verts_per_strip: %d", chunk->strips_num, chunk->verts_per_strip);
      
        nk_tree_pop(ctx);
      }
      
      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "layout", NK_MINIMIZED))
      {
        nk_property_int(ctx, "draw dist", 1, (int*)&core_data->terrain_draw_dist, 20, 1, 0.01f);
        nk_property_int(ctx, "cull dist", core_data->terrain_draw_dist +1, (int*)&core_data->terrain_cull_dist, 25, 1, 0.01f);
      
        nk_spacing(ctx, 1);
        nk_label(ctx, "add chunk", NK_TEXT_LEFT);

        static ivec2 pos = { 0, 0 };
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_property_int(ctx, "x", -1000, &pos[0], 1000, 1, 0.01f);
        nk_property_int(ctx, "z", -1000, &pos[1], 1000, 1, 0.01f);
        
        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "add chunk"))
        { 
          terrain_add_chunk(pos); 
        }

        nk_tree_pop(ctx);
      }
      
       if (nk_tree_push(ctx, NK_TREE_TAB, "materials", NK_MINIMIZED))
      {
        char buf[32];
        for (u32 i = 0;  i < core_data->terrain_materials_len; ++i)
        {
          sprintf(buf, "materials[%d]", i);
          if (nk_tree_push(ctx, NK_TREE_TAB, buf, NK_MINIMIZED))
          {
            material_t* mat = assetm_get_material_by_idx(core_data->terrain_materials[i]);
            gui_properties_material(mat, core_data->terrain_materials[i]);
            nk_tree_pop(ctx);
          }
        }
        nk_tree_pop(ctx);
      }
    }
    else
    { nk_labelf(ctx, NK_TEXT_LEFT, "no entity selected"); }
  }
  if (is_selected)
  { nk_end(ctx); }
}
void gui_properties_transform(vec3 pos, vec3 rot, vec3 scl, bool* has_moved)
{

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "position", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 pos_old; vec3_copy(pos, pos_old);
  nk_property_float(ctx, "p.x", -2048.0f, &pos[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.y", -2048.0f, &pos[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.z", -2048.0f, &pos[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(pos_old, pos)) { *has_moved = true; }

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "rotation", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 rot_old; vec3_copy(rot, rot_old);
  nk_property_float(ctx, "r.x", -2048.0f, &rot[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "r.y", -2048.0f, &rot[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "r.z", -2048.0f, &rot[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(rot_old, rot)) { *has_moved = true; }

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "scale", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 scl_old; vec3_copy(scl, scl_old);
  nk_property_float(ctx, "s.x", -2048.0f, &scl[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "s.y", -2048.0f, &scl[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "s.z", -2048.0f, &scl[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(scl_old, scl)) { *has_moved = true; }
}
void gui_properties_material(material_t* mat, int idx)
{
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "idx: %d", idx);

  const int size = prop_win_rect.w / 2 - 20;
  nk_layout_row_static(ctx, size, size, 2);
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->albedo)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->normal)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->roughness)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->metallic)->handle));

  nk_layout_row_dynamic(ctx, 30, 1);
  gui_color_selector(mat->tint);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_property_float(ctx, "roughness", 0.0f, &mat->roughness_f, 1.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "metallic",  0.0f, &mat->metallic_f,  1.0f, 0.1f, 0.01f);

  nk_spacing(ctx, 1);
  nk_checkbox_label(ctx, "tile by scale", &mat->tile_by_scl);
  nk_property_float(ctx, "tile scale",  -10.0f, &mat->tile_scl,  10.0f, 0.1f, 0.01f);
  nk_layout_row_dynamic(ctx, 30, 2);
  nk_property_float(ctx, "tile.x",  -10.0f, &mat->tile[0],  10.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "tile.y",  -10.0f, &mat->tile[1],  10.0f, 0.1f, 0.01f);
  nk_layout_row_dynamic(ctx, 30, 1);

  nk_spacing(ctx, 1);
  if (nk_button_label(ctx, "print template"))
  {
    ASSERT(mat->template_idx > 0);
#ifndef _MSC_VER
    material_template_t m = (material_template_t)( *material_template_get(mat->template_idx) );
#else
    material_template_t m = *material_template_get(mat->template_idx);
#endif
    vec3_copy(mat->tint, m.tint);
    m.roughn_f    = mat->roughness_f;
    m.metall_f    = mat->metallic_f;
    m.tile_scl    = mat->tile_scl;
    m.tile_by_scl = mat->tile_by_scl;
    vec2_copy(mat->tile, m.tile);
    const char* str = material_template_generate_string(&m);
    PF("%s", str);
  }
}
void gui_properties_mesh(mesh_t* m, int idx, int entity_template_idx)
{  
  const entity_template_t* tmplt = entity_template_get(entity_template_idx);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "name:    %s", tmplt->mesh);
  nk_labelf(ctx, NK_LEFT, "f32 per vert: %d", m->floats_per_vert);
  nk_labelf(ctx, NK_LEFT, "verts:   %d", m->verts_count);
  nk_labelf(ctx, NK_LEFT, "indices: %d", m->indices_count);
  
}
void gui_properties_point_light(point_light_t* p, int idx)
{
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "id: %d, arr idx: %d", p->id, idx);
  
  nk_label(ctx, "position offset", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  nk_property_float(ctx, "p.x", -2048.0f, &p->offset[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.y", -2048.0f, &p->offset[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.z", -2048.0f, &p->offset[2], 2048.0f, 0.1f, 0.01f);
  // nk_labelf(ctx, NK_LEFT, "p.x %.2f", (*p->pos_ptr)[0]);
  // nk_labelf(ctx, NK_LEFT, "p.y %.2f", (*p->pos_ptr)[1]);
  // nk_labelf(ctx, NK_LEFT, "p.z %.2f", (*p->pos_ptr)[2]);

  gui_color_selector(p->color);
  
  nk_property_float(ctx, "intensity", -2.0f, &p->intensity, 2048.0f, 0.1f, 0.01f);
  
}
void gui_properties_physics(const entity_template_t* def, entity_t* e)
{
  // @NOTE: tmp
  u32 arr_len = 0;
  phys_obj_t* arr = phys_get_obj_arr(&arr_len);
  phys_obj_t* obj = NULL;
  for (u32 i = 0; i < arr_len; ++i)
  {
    if (arr[i].entity_idx == e->id) { obj = &arr[i]; }
  }
  if (obj)
  {
    nk_layout_row_dynamic(ctx, 30, 1);
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- rigidbody --");
      
      nk_property_float(ctx, "mass", 0.1f, &obj->rb.mass, 4096.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "drag", 0.0001f, &obj->rb.drag, 2.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "friction", 0.0001f, &obj->rb.friction, 2.0f, 0.1f, 0.01f); 

      nk_labelf(ctx, NK_TEXT_LEFT, "pos: %f, %f, %f", obj->pos[0], obj->pos[1], obj->pos[2]);
      nk_labelf(ctx, NK_TEXT_LEFT, "vel: %f, %f, %f", obj->rb.velocity[0], obj->rb.velocity[1], obj->rb.velocity[2]);
      // nk_labelf(ctx, NK_TEXT_LEFT, "f:   %f, %f, %f", obj->rb.force[0], obj->rb.force[1], obj->rb.force[2]);
      
      nk_property_float(ctx, "force.x", -2048.0f, &obj->rb.force[0], 2048.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "force.y", -2048.0f, &obj->rb.force[1], 2048.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "force.z", -2048.0f, &obj->rb.force[2], 2048.0f, 0.1f, 0.01f);

      nk_labelf(ctx, NK_TEXT_LEFT, "is_grounded: %s", STR_BOOL(obj->collider.is_grounded));

    }
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_SPHERE))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- sphere --");
      nk_labelf(ctx, NK_TEXT_LEFT, "radius: %.2f", def->radius);
      
      nk_labelf(ctx, NK_TEXT_LEFT, "is trigger: %s", STR_BOOL(def->is_trigger));
      
      phys_debug_draw_sphere_collider(obj);
    }
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_BOX))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- box --");
      nk_labelf(ctx, NK_TEXT_LEFT, "[aabb]   x: %.2f y: %.2f, z: %.2f", def->aabb_size[0], def->aabb_size[1], def->aabb_size[2]);
      nk_labelf(ctx, NK_TEXT_LEFT, "[offset] x: %.2f y: %.2f, z: %.2f", obj->collider.offset[0], obj->collider.offset[1], obj->collider.offset[2]);
      
      nk_labelf(ctx, NK_TEXT_LEFT, "is trigger: %s", STR_BOOL(def->is_trigger));
    
      // doesnt make much sense, just use obb
      // if (nk_button_label(ctx, "rotate box y"))
      // {
      //   phys_rotate_box_y(e->id);
      // }

      phys_debug_draw_box_collider(obj);

    }
  }

}

void gui_template_browser_win()
{
  int w, h;
  window_get_size(&w, &h);

  const char* win_name = "template browser";
  
  template_browser_minimized = nk_window_is_collapsed(ctx, win_name) ? 1 : 0;
  // less height because the window bar on top and below
  template_win_ratio.w = 1.0f - prop_win_ratio.w; 
  template_win_ratio.h = 0.2f; 
  template_win_ratio.x = 0.0f; 
  template_win_ratio.y = template_browser_minimized ? 1.0f : 0.8f;

  template_win_rect = nk_rect(template_win_ratio.x * w, template_win_ratio.y * h, template_win_ratio.w * w, template_win_ratio.h * h);
  if (template_browser_minimized) { template_win_rect.y -= 35; }
  if (nk_begin(ctx, win_name, template_win_rect, window_min_flags)) 
  {
  
    int len = 0;
    const entity_template_t* table = entity_template_get_all(&len);
    static int selected = -1;
    static vec3 cam_pos = { 0, 0, 10 };

    nk_layout_row_begin(ctx, NK_STATIC, template_win_rect.h -60, 2);
    {
      const float table_names_w =  template_win_rect.w * 0.8f -20;
      nk_layout_row_push(ctx, table_names_w);
      if (nk_group_begin(ctx, "entity_table_names", 0))
      {
        const int row_w = 110;
        nk_layout_row_static(ctx, 50, row_w, (int)table_names_w / row_w);
        char buf[12];
        for (int i = 0; i < len; ++i)
        {
          SPRINTF(12, buf, "ent_grp_%d", i);
          if (nk_group_begin(ctx, buf, NK_WINDOW_NO_SCROLLBAR))
          {
            nk_layout_row_begin(ctx, NK_STATIC, 35, 2);
            
            nk_layout_row_push(ctx, 20);
            if (nk_button_label(ctx, "+"))
            {
              vec3 front, pos;
              vec3_copy(core_data->cam.front, front); // camera_get_front(front);
              vec3_copy(core_data->cam.pos,   pos);   // camera_get_pos(pos);
              vec3_mul_f(front, 8.0f, front);
              vec3_add(front, pos, pos);
              int id = state_entity_add_from_template(pos, VEC3(0), VEC3(1), i);
              app_data->selected_id = id;
            }

            bool check = i == selected;
            bool check_old = check;
            nk_layout_row_push(ctx, 75);
            ui_rect bounds = nk_widget_bounds(ctx);
            nk_selectable_label(ctx, table[i].name, NK_TEXT_LEFT, &check);
            if (!check_old && check) { selected = i;  vec3_copy(VEC3_Z(10), cam_pos); }
            if (check_old && !check) { selected = -1; vec3_copy(VEC3_Z(10), cam_pos); }
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds))
            { nk_tooltip(ctx, table[i].name); }

            nk_group_end(ctx);
          }
        }
        nk_group_end(ctx);
      }
      nk_layout_row_push(ctx, template_win_rect.w * 0.2f -20);
      if (nk_group_begin(ctx, "entity_preview", NK_WINDOW_NO_SCROLLBAR))
      {
        if (selected > -1)
        {

          const entity_template_t* def = entity_template_get(selected);
          mesh_t*    mesh = assetm_get_mesh(def->mesh);
          texture_t* tex  = assetm_get_texture_by_idx(assetm_get_material(def->mat)->albedo);
          
          texture_t* bg   = assetm_get_texture("#internal/preview_bg.png", true);
          
          renderer_direct_draw_mesh_preview(cam_pos, VEC3(0), VEC3(0), VEC3(1), mesh, tex, VEC3(1), bg, &fb_preview);
          // const int size = (template_win_rect.w * 0.2f) - 20;
          const int size = (template_win_rect.h) - 70;
          nk_layout_row_static(ctx, size, size, 1);    
          ui_rect img_bounds = nk_widget_bounds(ctx);
          nk_image(ctx, nk_image_id(fb_preview.buffer));

          // mouse drag for moving
          static bool dragging = false;
          if (input_get_mouse_down(MOUSE_LEFT) && (nk_input_is_mouse_hovering_rect(&ctx->input, img_bounds) || dragging))
          {
            dragging = true;
            double x, y;
            input_get_mouse_delta(&x, &y);
            cam_pos[0] += x * -0.002f;
            cam_pos[1] += y *  0.002f;
          } 
          else { dragging = false; }
          // mouse over scroll for zoom
          if (ctx->input.mouse.scroll_delta.y != 0 && nk_input_is_mouse_hovering_rect(&ctx->input, img_bounds))
          { cam_pos[2] += ctx->input.mouse.scroll_delta.y * 0.2f; }

        }
        nk_group_end(ctx);
      }
    }nk_layout_row_end(ctx);
      
  }
  nk_end(ctx);
 
  // minimize on startup
  static bool minimize = true;
  if (minimize)
  {  nk_window_collapse(ctx, win_name, NK_MINIMIZED); minimize = false; }
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

void gui_core_data_win()
{
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  const f32 w_ratio = 400.0f  / 1920.0f;
  const f32 h_ratio = 1000.0f / 1020.0f;
  const f32 x_ratio = 0.0f    / 1920.0f;
  const f32 y_ratio = 10.0f   / 1020.0f + top_bar_win_ratio.h;

  core_data_win_rect = nk_rect(x_ratio * w, y_ratio * h, w_ratio * w, h_ratio * h);
  if (nk_begin(ctx, "core_data", core_data_win_rect, window_float_flags)) 
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
    gui_shader_properties(&core_data->basic_shader, "basic_shader");
    gui_shader_properties(&core_data->shadow_shader, "shadow_shader");
    gui_shader_properties(&core_data->deferred_shader, "deferred_shader");
    gui_shader_properties(&core_data->lighting_shader, "lighting_shader");
    gui_shader_properties(&core_data->shadow_pass_shader, "shadow_pass_shader");
    gui_shader_properties(&core_data->skybox_shader, "skybox_shader");
    gui_shader_properties(&core_data->post_fx_shader, "post_fx_shader");
    gui_shader_properties(&core_data->brdf_lut_shader, "brdf_lut_shader");
    gui_shader_properties(&core_data->mouse_pick_shader, "mouse_pick_shader");


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
    gui_shader_properties(&core_data->equirect_shader, "equirect_shader");
    gui_shader_properties(&core_data->irradiance_map_shader, "irrandiance_map_shader");
    gui_shader_properties(&core_data->prefilter_shader, "prefilter_shader");
    
    nk_spacing(ctx, 1);
    gui_shader_properties(&core_data->terrain_shader, "terrain_shader");
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

void gui_shader_properties(shader_t* s, char* name)
{
  nk_labelf(ctx, NK_TEXT_LEFT, "%s:", name);
  nk_labelf(ctx, NK_TEXT_LEFT, "has_error: %s", STR_BOOL(s->has_error));
  nk_labelf(ctx, NK_TEXT_LEFT, "handle: %d", s->handle);
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

