#ifndef DEBUG_DRAW_H 
#define DEBUG_DRAW_H 

#ifdef DEBUG_DRAW

#include "global/global.h"
#include "core/debug/debug_draw.h"
#include "math/math_inc.h"

typedef enum debug_draw_type { DEBUG_DRAW_SPHERE, DEBUG_DRAW_LINE, DEBUG_DRAW_MESH, DEBUG_DRAW_MESH_TEX } debug_draw_type;

typedef struct debug_draw_t
{
  debug_draw_type type;
  bool is_model;
  mat4 model;
  vec3 pos;
  vec3 rot;
  vec3 scl;
  rgbf tint;
  int mesh;
  int tex;

}debug_draw_t;


#define DEBUG_DEFAULT_LINE_WIDTH 4
#define DEBUG_DEFAULT_BOX_WIDTH  DEBUG_DEFAULT_LINE_WIDTH

void debug_draw_init_func();
void debug_draw_update_func();
#define debug_draw_init()   debug_draw_init_func() 
#define debug_draw_update() debug_draw_update_func() 

void debug_draw_sphere_register_func(vec3 pos, float scl, rgbf tint);
void debug_draw_sphere_register_model_func(mat4 model, float scl, rgbf tint);

void debug_draw_line_register_func(vec3 pos0, vec3 po1, rgbf tint);
void debug_draw_line_register_width_func(vec3 pos0, vec3 pos1, rgbf tint, f32 width);

void debug_draw_mesh_register_func(vec3 pos, vec3 rot, vec3 scl, rgbf tint, int mesh);
void debug_draw_mesh_register_model_func(mat4 model, rgbf tint, int mesh);
void debug_draw_mesh_textured_register_model_func(mat4 model, rgbf tint, int mesh, int tex);

void debug_draw_box_register_func(vec3 points[8], rgbf color);
void debug_draw_box_register_width_func(vec3 points[8], rgbf color, f32 width);

#define debug_draw_sphere_register(pos, scl, tint)                       debug_draw_sphere_register_func(pos, scl, tint) 
#define debug_draw_sphere_register_model(model, scl, tint)               debug_draw_sphere_register_model_func(model, scl, tint) 

#define debug_draw_line_register(pos0, pos1, tint)                       debug_draw_line_register_func(pos0, pos1, tint)
#define debug_draw_line_register_width(pos0, pos1, tint, width)          debug_draw_line_register_width_func(pos0, pos1, tint, width)

#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)              debug_draw_mesh_register_func(pos, rot, scl, tint, mesh) 
#define debug_draw_mesh_register_model(model, tint, mesh)                debug_draw_mesh_register_model_func(model, tint, mesh) 
#define debug_draw_mesh_textured_register_model(model, tint, mesh, tex)  debug_draw_mesh_textured_register_model_func(model, tint, mesh, tex)       

#define debug_draw_box_register(points, color)                           debug_draw_box_register_func(points, color)
#define debug_draw_box_register_width(points, color, width)              debug_draw_box_register_width_func(points, color, width)

#else

// define as nothing to strip out debug code

#define debug_draw_init()   
#define debug_draw_update() 

#define debug_draw_sphere_register(pos, scl, tint)                       
#define debug_draw_sphere_register_model(model, scl, tint)               
#define debug_draw_line_register(pos0, pos1, tint)                       
#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)               
#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)              
#define debug_draw_mesh_register_model(model, tint, mesh)                
#define debug_draw_mesh_textured_register_model(model, tint, mesh, tex)  
#define debug_draw_box_register(points, color)                           
#define debug_draw_box_register_width(points, color, width)              

#endif

#endif
