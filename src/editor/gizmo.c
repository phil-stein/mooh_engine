#include "editor/gizmo.h"
#include "editor/app.h"
#include "core/window.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/debug/debug_draw.h"
#include "core/state.h"
#include "core/assetm.h"


// TODO: get static pointer
// static app_data_t* app_data= NULL; 

vec3 delta_pos = { 0 };
vec3 delta_rot = { 0 };
vec3 delta_scl = { 0 };


void gizmo_update()
{
  app_data_t*  app_data  = app_data_get();
  // core_data_t* core_data = core_data_get();
  
  // -- draw gizmo --
  
  if (app_data->selected_id >= 0 || app_data->selected_id == -2) // entity or terrain
  {
    mat4 model;
    vec3 pos;
    GIZMO_MODEL_POS(app_data, model, pos);

    
    if (app_data->gizmo_type == GIZMO_NONE)
    {
      debug_draw_sphere_register(pos, 0.25f, RGB_F(1.0f, 0.5f, 0.0f));
    }
    else if (app_data->gizmo_type == GIZMO_TRANSLATE)
    {
      debug_draw_mesh_textured_register_model(model, RGB_F_RGB(1.0f), assetm_get_mesh_idx("gizmos/translate/gizmo.fbx"), assetm_get_texture_idx("#internal/gizmo_atlas.png", true));
    }
    else if (app_data->gizmo_type == GIZMO_SCALE)
    {
      debug_draw_mesh_textured_register_model(model, RGB_F_RGB(1.0f), assetm_get_mesh_idx("gizmos/scale/gizmo.fbx"), assetm_get_texture_idx("#internal/gizmo_atlas.png", true));
    }    
    else if (app_data->gizmo_type == GIZMO_ROTATE)
    {
      debug_draw_mesh_textured_register_model(model, RGB_F_RGB(1.0f), assetm_get_mesh_idx("gizmos/rotate/gizmo.fbx"), assetm_get_texture_idx("#internal/gizmo_atlas.png", true));
    }
  }

  // -- gizmo logic --

  if (app_data->switch_gizmos_act)
  {
    if (input_get_key_pressed(KEY_GIZMO_TRANSLATE))   // translate
    { app_data->gizmo_type = app_data->gizmo_type == GIZMO_TRANSLATE ? GIZMO_NONE : GIZMO_TRANSLATE; } 
     if (input_get_key_pressed(KEY_GIZMO_SCALE))  // scale
    { app_data->gizmo_type = app_data->gizmo_type == GIZMO_SCALE     ? GIZMO_NONE : GIZMO_SCALE; }  
     if (input_get_key_pressed(KEY_GIZMO_ROTATE))  // scale
    { app_data->gizmo_type = app_data->gizmo_type == GIZMO_ROTATE    ? GIZMO_NONE : GIZMO_ROTATE; }  
  }

  static int gizmo_axis = -1;
  if (!app_data->mouse_over_ui && input_get_mouse_pressed(MOUSE_LEFT))
  {
    int id = renderer_mouse_position_mouse_pick_id(); 

    // P_INT(id);
    if (id >= 0)
    { 
      app_data->selected_id = id;
      gizmo_axis = -1;
    }
    else if (id == -1)  // nothing
    {
      gizmo_axis = -1; 
      app_data->selected_id = -1;
    }
    else if (id <= ID_BUFFER_TERRAIN_0)  // terrain 
    { 
      gizmo_axis = -1;
      app_data->gizmo_type = GIZMO_NONE;
      app_data->selected_id = id;
    }
    else if (app_data->gizmo_type == GIZMO_TRANSLATE)
    { 
      if (id == ID_BUFFER_GIZMO_0) // gizmo x
      { gizmo_axis = 0; }
      else if (id == ID_BUFFER_GIZMO_0 -1) // gizmo y
      { gizmo_axis = 1; }
      else if (id == ID_BUFFER_GIZMO_0 -2) // gizmo z
      { gizmo_axis = 2; }
      else if (id == ID_BUFFER_GIZMO_0 -3) // gizmo xy
      { gizmo_axis = 3; }
      else if (id == ID_BUFFER_GIZMO_0 -4) // gizmo xz
      { gizmo_axis = 4; }
      else if (id == ID_BUFFER_GIZMO_0 -5) // gizmo yz
      { gizmo_axis = 5; }
      else if (id == ID_BUFFER_GIZMO_0 -6) // gizmo xyz
      { gizmo_axis = 6; }
    }
    else if (app_data->gizmo_type == GIZMO_SCALE)
    {
      if (id == ID_BUFFER_GIZMO_0) // gizmo x
      { gizmo_axis = 0; }
      else if (id == ID_BUFFER_GIZMO_0 -1) // gizmo y
      { gizmo_axis = 1; }
      else if (id == ID_BUFFER_GIZMO_0 -2) // gizmo z
      { gizmo_axis = 2; }
      else if (id == ID_BUFFER_GIZMO_0 -3) // gizmo xyz
      { gizmo_axis = 3; }
    }
    else if (app_data->gizmo_type == GIZMO_ROTATE) 
    {
      if (id == ID_BUFFER_GIZMO_0) // gizmo x
      { gizmo_axis = 0; }
      else if (id == ID_BUFFER_GIZMO_0 -1) // gizmo y
      { gizmo_axis = 1; }
      else if (id == ID_BUFFER_GIZMO_0 -2) // gizmo z
      { gizmo_axis = 2; }
      else if (id == ID_BUFFER_GIZMO_0 -3) // gizmo xyz
      { gizmo_axis = 3; }
    }
    else
    {
      gizmo_axis = -1; 
      app_data->selected_id = -1;
    }
  }
  if (gizmo_axis >= GIZMO_NONE && input_get_mouse_down(MOUSE_LEFT))
  {
    int w, h; window_get_size(&w, &h);    
    double x, y;
    input_get_mouse_pos(&x, &y);   
    double dx, dy;
    input_get_mouse_delta(&dx, &dy);

    vec2 p0 = { x + dx, y - dy };
    vec2 p1 = { x, y };
    
    mat4 model;
    vec3 pos;
    GIZMO_MODEL_POS(app_data, model, pos);
    // if (app_data->selected_id >= 0)
    // {
    //   bool error = false;
    //   entity_t* e = state_get_entity(app_data->selected_id, &error); assert(!error);
    //   mat4_get_pos(e->model, pos);
    //   state_entity_model_no_scale(app_data->selected_id, model);
    // }
    // else // terrain
    // {
    //   mat4_make_model(core_data->terrain_pos, core_data->terrain_rot, VEC3(1), model);
    // }

    vec3 dist;
    gizmo_calc_dist_screen_to_model(p0, p1, pos, model, dist);

    if (app_data->gizmo_type == GIZMO_TRANSLATE)
    {
      vec3_mul_f(dist, app_data->gizmo_translate_speed, dist);
      if (gizmo_axis <= 2)
      { 
        delta_pos[gizmo_axis] += dist[gizmo_axis]; 
      }
      else if (gizmo_axis == 3) // xy
      { 
        delta_pos[0] += dist[0]; 
        delta_pos[1] += dist[1]; 
      }
      else if (gizmo_axis == 4) // xz
      {  
        delta_pos[0] += dist[0]; 
        delta_pos[2] += dist[2]; 
      }
      else if (gizmo_axis == 5) // yz
      {  
        delta_pos[1] += dist[1]; 
        delta_pos[2] += dist[2]; 
      }
      else if (gizmo_axis == 6) // xyz
      {  
        vec3_add(delta_pos, dist, delta_pos); 
      }
    }
    if (app_data->gizmo_type == GIZMO_SCALE)
    {
      vec3_mul_f(dist, app_data->gizmo_scale_speed, dist);

      if (gizmo_axis <= 2)
      { 
        f32 delta = dist[0] + dist[1] + dist[2];
        delta_scl[gizmo_axis] += delta; 
      }
      if (gizmo_axis == 3) // xyz
      { 
        f32 delta = dist[0] + dist[1] + dist[2];
        vec3_add(delta_scl, VEC3(delta), delta_scl); 
      }
    }
    if (app_data->gizmo_type == GIZMO_ROTATE)
    {
      vec3_mul_f(dist, app_data->gizmo_rotate_speed, dist);
      if (gizmo_axis <= 2)
      { 
        f32 delta = dist[0] + dist[1] + dist[2];
        delta_rot[gizmo_axis] += delta; 
      }
      else if (gizmo_axis == 3) // xyz
      {  
        vec3_add(delta_rot, dist, delta_rot); 
      }
    }

    // -- apply change --

    if (app_data->selected_id >= 0) // entity
    {
      bool err = false;
      entity_t* e = state_get_entity(app_data->selected_id, &err);
      assert(!err);

      if (app_data->gizmo_snapping)
      {
        // -- translation --
        if (fabsf(delta_pos[0]) >= app_data->gizmo_translate_snap)
        {
          ENTITY_MOVE_X(e, delta_pos[0]);
          delta_pos[0] = 0;
        }
        if (fabsf(delta_pos[1]) >= app_data->gizmo_translate_snap)
        {
          ENTITY_MOVE_Y(e, delta_pos[1]);
          delta_pos[1] = 0;
        }
        if (fabsf(delta_pos[2]) >= app_data->gizmo_translate_snap)
        {
          ENTITY_MOVE_Z(e, delta_pos[2]);
          delta_pos[2] = 0;
        }
        // -- rotation --
        if (fabsf(delta_rot[0]) >= app_data->gizmo_rotate_snap)
        {
          ENTITY_ROTATE_X(e, delta_rot[0]);
          delta_rot[0] = 0;
        }
        if (fabsf(delta_rot[1]) >= app_data->gizmo_rotate_snap)
        {
          ENTITY_ROTATE_Y(e, delta_rot[1]);
          delta_rot[1] = 0;
        }
        if (fabsf(delta_rot[2]) >= app_data->gizmo_rotate_snap)
        {
          ENTITY_ROTATE_Z(e, delta_rot[2]);
          delta_rot[2] = 0;
        }
        // -- scale --
        if (fabsf(delta_scl[0]) >= app_data->gizmo_scale_snap)
        {
          ENTITY_SCALE_X(e, delta_scl[0]);
          delta_scl[0] = 0;
        }
        if (fabsf(delta_scl[1]) >= app_data->gizmo_scale_snap)
        {
          ENTITY_SCALE_Y(e, delta_scl[1]);
          delta_scl[1] = 0;
        }
        if (fabsf(delta_scl[2]) >= app_data->gizmo_scale_snap)
        {
          ENTITY_SCALE_Z(e, delta_scl[2]);
          delta_scl[2] = 0;
        }
      }
      else 
      {
        ENTITY_MOVE(e,   delta_pos);
        ENTITY_ROTATE(e, delta_rot);
        ENTITY_SCALE(e,  delta_scl);
        vec3_copy(VEC3(0), delta_pos);
        vec3_copy(VEC3(0), delta_rot);
        vec3_copy(VEC3(0), delta_scl);
      }
    }

  }

}


// 3d distance between 2 screen-space points projected into and entity's model space
// p0 & p1: screen-space, in pixel, coordinates
// e: entity defining the model-space used
void gizmo_calc_dist_screen_to_model(vec2 p0, vec2 p1, vec3 entity_pos, mat4 entity_model, vec3 out)
{
  int w, h;
  window_get_size(&w, &h);
  mat4 view, proj;
  camera_get_view_mat(view);
  camera_get_proj_mat(w, h, proj);

  // pos0 ---------------------------------------------------------------------
  
  vec3 pos0;

  vec2 pos_norm = 
  {
     (p0[0] / w) * 2 -1,
    -(p0[1] / h) * 2 -1
  };
  vec3 cam_pos, dist;
  camera_get_pos(cam_pos);
  vec3_sub(entity_pos, cam_pos, dist);
  float depth = vec3_magnitude(dist);
  depth = (depth / camera_get_f_plane()) * 2 -1;
  space_screen_to_world(view, proj, pos_norm, depth, pos0); 

  mat4 model;
  mat4_make_model(pos0, VEC3(0), VEC3(1), model);
  mat4_mul(model, entity_model, model);
  mat4_get_pos(model, pos0);
  
  // pos1 ---------------------------------------------------------------------

  vec3 pos1;

  vec2_copy(
    VEC2_XY( 
     (p1[0] / w) * 2 -1,
    -(p1[1] / h) * 2 -1),
    pos_norm);

  space_screen_to_world(view, proj, pos_norm, depth, pos1); 

  mat4_make_model(pos1, VEC3(0), VEC3(1), model);
  mat4_mul(model, entity_model, model);
  mat4_get_pos(model, pos1);

  // distance -----------------------------------------------------------------
  
  vec3_sub(pos0, pos1, out);
  float d = vec3_magnitude(dist);   // dist: cam_pos - pos
  vec3_mul_f(out, d, out);
}
