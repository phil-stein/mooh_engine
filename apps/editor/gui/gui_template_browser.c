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
#include "editor/operation.h"
#include "core/core_data.h"
#include "core/state.h"
#include "core/io/assetm.h"
#include "core/renderer/renderer_direct.h"


void gui_template_browser_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags)
{
  // int w, h;
  // window_get_size(&w, &h);
    
  char* win_name = "template browser";

  core_data_t* core_data = core_data_get();
  app_data_t*  app_data  = app_data_get();
  
  app_data->template_browser_minimized = nk_window_is_collapsed(ctx, win_name) ? 1 : 0;

  if (app_data->template_browser_minimized) { win_rect.y -= 35; }
  if (nk_begin(ctx, win_name, win_rect, win_flags)) 
  {
  
    int len = 0;
    const entity_template_t* table = entity_template_get_all(&len);
    static int selected = -1;
    static vec3 cam_pos = { 0, 0, 10 };

    nk_layout_row_begin(ctx, NK_STATIC, win_rect.h -60, 2);
    {
      const float table_names_w =  win_rect.w * 0.8f -20;
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
              
              operation_t op = OPERATION_T_ENTITY_ADD(id);
              operation_register(&op);
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
      nk_layout_row_push(ctx, win_rect.w * 0.2f -20);
      if (nk_group_begin(ctx, "entity_preview", NK_WINDOW_NO_SCROLLBAR))
      {
        if (selected > -1)
        {

          const entity_template_t* def = entity_template_get(selected);
          mesh_t*    mesh = assetm_get_mesh(def->mesh);
          texture_t* tex  = assetm_get_texture_by_idx(assetm_get_material(def->mat)->albedo);
          
          texture_t* bg   = assetm_get_texture("#internal/preview_bg.png", true);
          
          renderer_direct_draw_mesh_preview(cam_pos, VEC3(0), VEC3(0), VEC3(1), mesh, tex, VEC3(1), bg, &app_data->fb_preview);
          // const int size = (win_rect.w * 0.2f) - 20;
          const int size = (win_rect.h) - 70;
          nk_layout_row_static(ctx, size, size, 1);    
          ui_rect img_bounds = nk_widget_bounds(ctx);
          nk_image(ctx, nk_image_id(app_data->fb_preview.buffer));

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
