#ifdef DEBUG_DRAW

#include "core/debug/debug_draw.h"
#include "core/assetm.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"

#include "stb/stb_ds.h"


debug_draw_t* queue = NULL;
int           queue_len = 0;

int blank_tex;
int sphere_mesh;


void debug_draw_init_func()
{
  blank_tex   = assetm_get_texture_idx("#internal/blank.png", true);
  sphere_mesh = assetm_get_mesh_idx("sphere.fbx");
}

void debug_draw_update_func()
{
  texture_t* t = assetm_get_texture_by_idx(blank_tex);
  for (int i = 0; i < queue_len; ++i)
  {
      if (queue[i].type == DEBUG_DRAW_SPHERE)
      {
        mesh_t* m = assetm_get_mesh("sphere.fbx");
        if (queue[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue[i].model, m, t, queue[i].tint); }
        else
        { renderer_direct_draw_mesh_textured(queue[i].pos, queue[i].rot, queue[i].scl, m, t, queue[i].tint); }
      }
      else if (queue[i].type == DEBUG_DRAW_LINE)
      {
        renderer_direct_draw_line(queue[i].pos, queue[i].rot, queue[i].tint, queue[i].scl[0]);  // using rot as pos2
      }
      else if (queue[i].type == DEBUG_DRAW_MESH)
      {
        mesh_t* m = assetm_get_mesh_by_idx(queue[i].mesh);
        if (queue[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue[i].model, m, t, queue[i].tint); }
        else 
        { renderer_direct_draw_mesh_textured(queue[i].pos, queue[i].rot, queue[i].scl, m, t, queue[i].tint); }
      }
      else if (queue[i].type == DEBUG_DRAW_MESH_TEX)
      {
        mesh_t*      m = assetm_get_mesh_by_idx(queue[i].mesh);
        texture_t* tex = assetm_get_texture_by_idx(queue[i].tex);
        if (queue[i].is_model)
        { renderer_direct_draw_mesh_textured_mat(queue[i].model, m, tex, queue[i].tint); }
        else 
        { renderer_direct_draw_mesh_textured(queue[i].pos, queue[i].rot, queue[i].scl, m, tex, queue[i].tint); }
      }

  }
  arrfree(queue);
  queue = NULL;
  queue_len = 0;  // reset for next frame
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

  arrput(queue, d);
  queue_len++;
}

void debug_draw_sphere_register_model_func(mat4 model, float scl, rgbf tint)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_SPHERE;
  d.is_model = true;
  mat4_copy(model, d.model);
  vec3_copy(tint, d.tint);

  arrput(queue, d);
  queue_len++;
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

  arrput(queue, d);
  queue_len++;
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

  arrput(queue, d);
  queue_len++;
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

  arrput(queue, d);
  queue_len++;
}

void debug_draw_mesh_register_model_func(mat4 model, rgbf tint, int mesh)
{
  debug_draw_t d;
  d.type = DEBUG_DRAW_MESH;
  d.is_model = true;
  mat4_copy(model, d.model);
  vec3_copy(tint, d.tint);
  d.mesh = mesh;

  arrput(queue, d);
  queue_len++;
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

  arrput(queue, d);
  queue_len++;
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
#endif
