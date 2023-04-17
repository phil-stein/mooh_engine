#ifdef DEBUG_DRAW

#include "core/debug/debug_draw.h"
#include "core/io/assetm.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"

#include "stb/stb_ds.h"


debug_draw_t* queue_arr = NULL;
int           queue_arr_len = 0;

int blank_tex;
int sphere_mesh;


void debug_draw_init_func()
{
  blank_tex   = assetm_get_texture_idx("#internal/blank.png", true);
  sphere_mesh = assetm_get_mesh_idx("sphere");
}

void debug_draw_update_func()
{
  texture_t* t = assetm_get_texture_by_idx(blank_tex);
  for (int i = 0; i < queue_arr_len; ++i)
  {
      if (queue_arr[i].type == DEBUG_DRAW_SPHERE)
      {
        mesh_t* m = assetm_get_mesh("sphere");
        if (queue_arr[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue_arr[i].model, m, t, queue_arr[i].tint); }
        else
        { renderer_direct_draw_mesh_textured(queue_arr[i].pos, queue_arr[i].rot, queue_arr[i].scl, m, t, queue_arr[i].tint); }
      }
      else if (queue_arr[i].type == DEBUG_DRAW_LINE)
      {
        renderer_direct_draw_line(queue_arr[i].pos, queue_arr[i].rot, queue_arr[i].tint, queue_arr[i].scl[0]);  // using rot as pos2
      }
      else if (queue_arr[i].type == DEBUG_DRAW_MESH)
      {
        mesh_t* m = assetm_get_mesh_by_idx(queue_arr[i].mesh);
        if (queue_arr[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue_arr[i].model, m, t, queue_arr[i].tint); }
        else 
        { renderer_direct_draw_mesh_textured(queue_arr[i].pos, queue_arr[i].rot, queue_arr[i].scl, m, t, queue_arr[i].tint); }
      }
      else if (queue_arr[i].type == DEBUG_DRAW_MESH_TEX)
      {
        mesh_t*      m = assetm_get_mesh_by_idx(queue_arr[i].mesh);
        texture_t* tex = assetm_get_texture_by_idx(queue_arr[i].tex);
        if (queue_arr[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue_arr[i].model, m, tex, queue_arr[i].tint); }
        else 
        { renderer_direct_draw_mesh_textured(queue_arr[i].pos, queue_arr[i].rot, queue_arr[i].scl, m, tex, queue_arr[i].tint); }
      }

  }
  ARRFREE(queue_arr);
  queue_arr = NULL;
  queue_arr_len = 0;  // reset for next frame
}

void debug_draw_sphere_register_func(vec3 pos, float scl, rgbf tint)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_SPHERE;
  d.is_model = false;
  vec3_copy(pos, d.pos);
  vec3_copy(VEC3(0), d.rot);
  vec3_copy(VEC3(scl), d.scl);
  vec3_copy(tint, d.tint);

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_sphere_register_model_func(mat4 model, float scl, rgbf tint)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_SPHERE;
  d.is_model = true;
  mat4_copy(model, d.model);
  vec3_copy(tint, d.tint);

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_line_register_func(vec3 pos0, vec3 pos1, rgbf tint)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_LINE;
  d.is_model = false;
  vec3_copy(pos0, d.pos);
  vec3_copy(pos1, d.rot);
  vec3_copy(VEC3(DEBUG_DEFAULT_LINE_WIDTH), d.scl);  // default width
  vec3_copy(tint, d.tint);

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_line_register_width_func(vec3 pos0, vec3 pos1, rgbf tint, f32 width)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_LINE;
  d.is_model = false;
  vec3_copy(pos0, d.pos);
  vec3_copy(pos1, d.rot);
  vec3_copy(VEC3(width), d.scl);
  vec3_copy(tint, d.tint);

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_mesh_register_func(vec3 pos, vec3 rot, vec3 scl, rgbf tint, int mesh)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_MESH;
  d.is_model = false;
  vec3_copy(pos, d.pos);
  vec3_copy(rot, d.rot);
  vec3_copy(scl, d.scl);
  vec3_copy(tint, d.tint);
  d.mesh = mesh;

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_mesh_register_model_func(mat4 model, rgbf tint, int mesh)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_MESH;
  d.is_model = true;
  mat4_copy(model, d.model);
  vec3_copy(tint, d.tint);
  d.mesh = mesh;

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_mesh_textured_register_func(vec3 pos, vec3 rot, vec3 scl, rgbf tint, int mesh, int tex)
{
  mat4 model;
  mat4_make_model(pos, rot, scl, model);
  debug_draw_mesh_textured_register_model(model, tint, mesh, tex);
}

void debug_draw_mesh_textured_register_model_func(mat4 model, rgbf tint, int mesh, int tex)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_MESH_TEX;
  d.is_model = true;
  mat4_copy(model, d.model);
  vec3_copy(tint, d.tint);
  d.mesh = mesh;
  d.tex  = tex;

  arrput(queue_arr, d);
  queue_arr_len++;
}

void debug_draw_box_register_func(vec3 points[8], rgbf color)
{
  debug_draw_line_register_width(points[0], points[1], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[1], points[2], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[2], points[3], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[3], points[0], color, DEBUG_DEFAULT_BOX_WIDTH); 
  
  debug_draw_line_register_width(points[4], points[5], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[5], points[6], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[6], points[7], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[7], points[4], color, DEBUG_DEFAULT_BOX_WIDTH); 
  
  debug_draw_line_register_width(points[4], points[0], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[5], points[1], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[6], points[2], color, DEBUG_DEFAULT_BOX_WIDTH); 
  debug_draw_line_register_width(points[7], points[3], color, DEBUG_DEFAULT_BOX_WIDTH); 
}

void debug_draw_box_register_width_func(vec3 points[8], rgbf color, f32 width)
{
  debug_draw_line_register_width(points[0], points[1], color, width); 
  debug_draw_line_register_width(points[1], points[2], color, width); 
  debug_draw_line_register_width(points[2], points[3], color, width); 
  debug_draw_line_register_width(points[3], points[0], color, width); 
  
  debug_draw_line_register_width(points[4], points[5], color, width); 
  debug_draw_line_register_width(points[5], points[6], color, width); 
  debug_draw_line_register_width(points[6], points[7], color, width); 
  debug_draw_line_register_width(points[7], points[4], color, width); 
  
  debug_draw_line_register_width(points[4], points[0], color, width); 
  debug_draw_line_register_width(points[5], points[1], color, width); 
  debug_draw_line_register_width(points[6], points[2], color, width); 
  debug_draw_line_register_width(points[7], points[3], color, width); 
}

// @TODO: make this instead of using fixed resolution circles
// @NOTE: testing different method
// void circle_test(vec3 plane, vec3 pos,  f32 radius, f32* color)
// {
//   // for vec3[axis_0], either x or y
//   u32 axis_0  = plane[0] > 0 ? 0 : 1;
//   // for vec3[axis_1], either y or z
//   u32 axis_1  = plane[1] > 0 && axis_0 != 1 ? 1 : 2;
// 
//   // line on x axis / axis_0 
//   vec3 axis = { 0, 0, 0 };
//   axis[axis_0] = radius;
//   vec3 p_axis_0_min, p_axis_0_max;
//   vec3_copy(pos, p_axis_0_min);
//   vec3_copy(pos, p_axis_0_max);
//   vec3_sub(p_axis_0_min, axis, p_axis_0_min);
//   vec3_add(p_axis_0_max, axis, p_axis_0_max);
//   
//   // line on y axis / axis_1
//   axis[axis_0] = 0.0f;  // back to ( 0, 0, 0 )
//   axis[axis_1] = radius;
//   vec3 p_axis_1_min, p_axis_1_max;
//   vec3_copy(pos, p_axis_1_min);
//   vec3_copy(pos, p_axis_1_max);
//   vec3_sub(p_axis_1_min, axis, p_axis_1_min);
//   vec3_add(p_axis_1_max, axis, p_axis_1_max);
//   
//   struct { vec3 p0; vec3 p1 }* line_arr = NULL;
//   u32 line_arr_len = 0;
// 
//   // oi u lazy fuckwit
// 
//   // also make this use line a line strip thingy
//   // not individual lines that are each a draw call
// 
//   int resolution = 3;
//   for (u32 i = 0; i < resolution; ++i)
//   {
//     // calc and add lines to arr for quarter circle
//     // resolution is how many lines per quarter circle
//   }
// 
//   // draw the quarter aain but flip
// 
// }
void circle_test(vec3 pos, vec3 rot,  f32 radius, u32 points, f32* color)
{
  // make 'parent' matrix with rot
  mat4 rot_m;
  mat4_make_identity(rot_m);
	f32 x = rot[0];  m_deg_to_rad(&x);
	f32 y = rot[1];  m_deg_to_rad(&y);
	f32 z = rot[2];  m_deg_to_rad(&z);
	mat4_rotate_at(rot_m, pos, x, VEC3_X(1));
	mat4_rotate_at(rot_m, pos, y, VEC3_Y(1));
	mat4_rotate_at(rot_m, pos, z, VEC3_Z(1));

  // rotate 360 deg
  f32 step_rad = 360.0f / (f32)points;
  m_deg_to_rad(&step_rad);
  mat4 circle_m;
  mat4_rotate_make(circle_m, step_rad, VEC3_Y(1));

  vec3 p0, p1;
  
  for (u32 i = 1; i < points; ++i)
  {
    // @TODO:  
    // rotate
    // draw line
    // switch pointers
  }
}
void debug_draw_draw_circle_func(vec3 plane, vec3 pos,  f32 radius, f32* color)
{
  // circle_test(plane, pos,  radius, color);
  // return;
  
  // for vec3[axis_0], either x or y
  u32 axis_0  = plane[0] > 0 ? 0 : 1;
  // for vec3[axis_1], either y or z
  u32 axis_1  = plane[1] > 0 && axis_0 != 1 ? 1 : 2;

  // line on x axis / axis_0 
  vec3 axis = { 0, 0, 0 };
  axis[axis_0] = radius;
  vec3 p_axis_0_min, p_axis_0_max;
  vec3_copy(pos, p_axis_0_min);
  vec3_copy(pos, p_axis_0_max);
  vec3_sub(p_axis_0_min, axis, p_axis_0_min);
  vec3_add(p_axis_0_max, axis, p_axis_0_max);
  
  // line on y axis / axis_1
  axis[axis_0] = 0.0f;  // back to ( 0, 0, 0 )
  axis[axis_1] = radius;
  vec3 p_axis_1_min, p_axis_1_max;
  vec3_copy(pos, p_axis_1_min);
  vec3_copy(pos, p_axis_1_max);
  vec3_sub(p_axis_1_min, axis, p_axis_1_min);
  vec3_add(p_axis_1_max, axis, p_axis_1_max);
  
  // use pythag to determine point on sphere, with radius and either x, y coord 
  // r^2 = x^2 + y^2
  // x = 0.5f
  // y = sqrt( r^2 - x^2 )
  // actually x, y here is axis_0 & axis_1
  vec3 mid_p_0 = { 0.0f, 0.0f, 0.0f};
  mid_p_0[axis_0] = radius * 0.5f;
  mid_p_0[axis_1] = F32_SQRT( pow(radius, 2) - pow(mid_p_0[axis_0], 2) );
 
  
  // y = 0.5f
  // x = sqrt( r^2 - y^2 )
  // actually x, y here is axis_0 & axis_1
  vec3 mid_p_1 = { 0.0f, 0.0f, 0.0f};
  mid_p_1[axis_1] = radius * 0.5f;
  mid_p_1[axis_0] = F32_SQRT( pow(radius, 2) - pow(mid_p_1[axis_1], 2) );
  
  // // y = -0.5f
  // // x = sqrt( r^2 - y^2 )
  // vec3 mid_p_y_flip_y = { 0.0f, -0.5f };
  // mid_p_y_flip_y[0] = F32_SQRT( pow(obj->collider.sphere.radius, 2) - pow(mid_p_y_flip_y[1], 2) );
  // vec3_add(mid_p_y_flip_y, obj->pos, mid_p_y_flip_y);
  vec3 mid_p_1_flip_1;
  vec3_copy(mid_p_1, mid_p_1_flip_1);
  mid_p_1_flip_1[axis_1] *= -1.0f; // neg. y / axis_1


  vec3 mid_p_0_flip_1;
  vec3_copy(mid_p_0, mid_p_0_flip_1);
  mid_p_0_flip_1[axis_1] *= -1.0f; // neg. y / axis_1

  vec3 mid_p_0_flip_01;
  vec3_copy(mid_p_0_flip_1, mid_p_0_flip_01);
  mid_p_0_flip_01[axis_0] *= -1.0f; // neg. x / axis_0
  
  vec3 mid_p_1_flip_01;
  vec3_copy(mid_p_1_flip_1, mid_p_1_flip_01);
  mid_p_1_flip_01[axis_0] *= -1.0f; // neg. x / axis_0
 
  vec3 mid_p_1_flip_0;
  vec3_copy(mid_p_1, mid_p_1_flip_0);
  mid_p_1_flip_0[axis_0] *= -1.0f; // neg. x / axis_0
  
  vec3 mid_p_0_flip_0;
  vec3_copy(mid_p_0, mid_p_0_flip_0);
  mid_p_0_flip_0[axis_0] *= -1.0f; // neg. x / axis_0

  vec3_add(mid_p_0, pos, mid_p_0);
  vec3_add(mid_p_1, pos, mid_p_1); 
  vec3_add(mid_p_1_flip_1, pos, mid_p_1_flip_1);
  vec3_add(mid_p_0_flip_1, pos, mid_p_0_flip_1);
  vec3_add(mid_p_0_flip_01, pos, mid_p_0_flip_01);
  vec3_add(mid_p_1_flip_01, pos, mid_p_1_flip_01);
  vec3_add(mid_p_1_flip_0,  pos, mid_p_1_flip_0);
  vec3_add(mid_p_0_flip_0,  pos, mid_p_0_flip_0);

  debug_draw_line_register(p_axis_1_max, mid_p_0, color);
  //
  debug_draw_line_register(mid_p_0, mid_p_1, color);
  debug_draw_line_register(mid_p_1, p_axis_0_max, color);
  //
  debug_draw_line_register(p_axis_0_max, mid_p_1_flip_1, color);
  //
  debug_draw_line_register(mid_p_1_flip_1, mid_p_0_flip_1, color);
  debug_draw_line_register(mid_p_0_flip_1, p_axis_1_min, color);
  //
  debug_draw_line_register(p_axis_1_min, mid_p_0_flip_01, color);
  //
  debug_draw_line_register(mid_p_0_flip_01, mid_p_1_flip_01, color);
  debug_draw_line_register(mid_p_1_flip_01, p_axis_0_min, color);
  //
  debug_draw_line_register(p_axis_0_min, mid_p_1_flip_0, color);
  //
  debug_draw_line_register(mid_p_1_flip_0, mid_p_0_flip_0, color);
  debug_draw_line_register(mid_p_0_flip_0, p_axis_1_max, color);

}

#endif
