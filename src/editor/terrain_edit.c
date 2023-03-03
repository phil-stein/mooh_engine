#include "editor/terrain_edit.h"
#include "editor/app.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/renderer_extra.h"
#include "core/types/types.h"
#include "core/debug/debug_draw.h"

#include "stb/stb_ds.h"

void terrain_edit_update()
{
  core_data_t* core_data = core_data_get();
  app_data_t*  app_data  = app_data_get();

  if (app_data->terrain_edit_type != TERRAIN_EDIT_NONE && 
      app_data->selected_id <= ID_BUFFER_TERRAIN_0 && !app_data->mouse_over_ui)
  {
    f64 x, y;
    input_get_mouse_pos(&x, &y);
  
    vec4 mouse_proj;
    framebuffer_get_rgbaf_value(&core_data->fb_deferred, 3, (int)x, (int)y, mouse_proj);
      
    debug_draw_sphere_register(mouse_proj, 0.25f, RGB_F(1.0f, 0.0f, 1.0f));    // projected mouse pos

    ERR_CHECK(app_data->selected_id > ID_BUFFER_TERRAIN_0 - core_data->terrain_layout_len, "selected id referencing terrain outside the bounds of the terrain array.");
    
    // int chunk_idx = ID_BUFFER_TERRAIN_TO_CHUNK_IDX(app_data->selected_id);
    for (u32 chunk_idx = 0; chunk_idx < core_data->terrain_chunks_len; ++chunk_idx)
    {
      terrain_chunk_t* chunk = &core_data->terrain_chunks[chunk_idx];

      // debug_draw_sphere_register(chunk->pos, 0.25f, RGB_F(1.0f, 0.5f, 0.0f)); // chunk origin / position

      // go throuugh all verts
      bool changed_verts = false;
      u32* smooth_verts = NULL;
      u32  smooth_verts_len = 0;
      f32  smooth_vert_y_sum = 0.0f;
      for (u32 i = 0; i < chunk->verts_len; i += TERRAIN_FLOATS_PER_VERT)
      {
        vec3 pos = { chunk->verts[i +0], chunk->verts[i +1], chunk->verts[i +2] };
        vec3_mul_f(pos, core_data->terrain_scl, pos);
        vec3_add(pos, chunk->pos, pos);
        
        // // draw corner verts
        // if (i == 0 ||                                                                                     // top-left
        //     i / TERRAIN_FLOATS_PER_VERT == core_data->terrain_x_len -1 ||                                         // top-right
        //     i == chunk->verts_len - TERRAIN_FLOATS_PER_VERT ||                                                    // bottom-right
        //     i == chunk->verts_len - TERRAIN_FLOATS_PER_VERT - ((core_data->terrain_x_len -1) * TERRAIN_FLOATS_PER_VERT) ) // bottom-left                   
        // { debug_draw_sphere_register(pos, 0.55f, RGB_F(1.0f, 0.5f, 0.0f)); } // vertex position

        // sculpting
        if (vec3_distance(pos, mouse_proj) <= app_data->terrain_edit_radius)
        { 
          debug_draw_sphere_register(pos, 0.15f, RGB_F(1.0f, 1.0f, 1.0f)); // vertex position
          if (input_get_mouse_down(MOUSE_LEFT))
          {
            if (app_data->terrain_edit_type == TERRAIN_EDIT_SCULPT)
            {
              int direction = input_get_key_down(KEY_LEFT_CONTROL) ? -1 : 1;
              chunk->verts[i +1] += (app_data->terrain_edit_radius - vec3_distance(pos, mouse_proj)) *  
                                    app_data->terrain_edit_scalar * app_data->terrain_edit_strength * core_data->delta_t * direction;
            }
            else if (app_data->terrain_edit_type == TERRAIN_EDIT_LEVEL)
            {
              chunk->verts[i +1] += (mouse_proj[1] - pos[1]) *
                                    app_data->terrain_edit_scalar * app_data->terrain_edit_strength * core_data->delta_t;
            }
            else if (app_data->terrain_edit_type == TERRAIN_EDIT_SMOOTH)
            {
              arrput(smooth_verts, i);
              smooth_verts_len++;
              smooth_vert_y_sum += pos[1];
            }
            else if (app_data->terrain_edit_type == TERRAIN_EDIT_PAINT)
            {
              f32 dir = app_data->terrain_edit_paint_material - chunk->verts[i +11];
              chunk->verts[i +11] += (app_data->terrain_edit_radius - vec3_distance(pos, mouse_proj)) * dir * app_data->terrain_edit_strength * core_data->delta_t * app_data->terrain_edit_paint_scalar;
              chunk->verts[i +11] = MIN(chunk->verts[i +11], 1.0f);
            }
            changed_verts = true;
          }
        } 
      }
      // smoothing 
      for (int i = 0; i < smooth_verts_len; ++i)
      {
        u32 x_len = core_data->terrain_x_len; 
        u32 idx0 = smooth_verts[i];
        f32 y_sum = 0.0f;
        u32 divisor = 0;
        for (int j = 0; j < smooth_verts_len; ++j)
        {
          u32 idx1 = smooth_verts[j];
          if (idx1 == idx0 + TERRAIN_FLOATS_PER_VERT) { y_sum += chunk->verts[idx1 +1]; divisor++; }            // left
          if (idx1 == idx0 - TERRAIN_FLOATS_PER_VERT) { y_sum += chunk->verts[idx1 +1]; divisor++; }            // right
          if (idx1 == idx0 + (x_len * TERRAIN_FLOATS_PER_VERT)) { y_sum += chunk->verts[idx1 +1]; divisor++; }  // top
          if (idx1 == idx0 - (x_len * TERRAIN_FLOATS_PER_VERT)) { y_sum += chunk->verts[idx1 +1]; divisor++; }  // bottom
        }
        if (divisor > 0)
        {
          y_sum = y_sum / divisor;
          y_sum -= chunk->verts[idx0 +1];
          chunk->verts[idx0 +1] += y_sum * app_data->terrain_edit_smooth_scalar * app_data->terrain_edit_strength * core_data->delta_t;
        }
      }
      arrfree(smooth_verts);

      if (changed_verts) { terrain_update_chunk_verts(chunk_idx); }
    }
  }
}

