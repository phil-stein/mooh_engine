#ifndef CORE_DEBUG_DRAW_H 
#define CORE_DEBUG_DRAW_H 

// @NOTE: if not defined all this gets stripped out
#ifdef DEBUG_DRAW

#include "../../global/global.h"
#include "../../core/debug/debug_draw.h"
#include "../../math/math_inc.h"

// @DOC: used internally in debug_draw_t
typedef enum debug_draw_type { DEBUG_DRAW_SPHERE, DEBUG_DRAW_LINE, DEBUG_DRAW_MESH, DEBUG_DRAW_MESH_TEX } debug_draw_type;

// @DOC: defines a debug_draw_... call, used internally
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


// @DOC: with of lines in draw_line_register() func's
#define DEBUG_DEFAULT_LINE_WIDTH 4
// @DOC: with of lines in draw_box_register() func's
#define DEBUG_DEFAULT_BOX_WIDTH  DEBUG_DEFAULT_LINE_WIDTH

// init&update ------------------------------------------------------------------------------------

// @DOC: init debug draw, call this before any other calls to debug draw 
void debug_draw_init_func();
// @DOC: call this every frame, renders the debug_draw calls, so call after renderer_update()
void debug_draw_update_func();
// @NOTE: macros so they can be compiled out 
#define debug_draw_init()   debug_draw_init_func() 
#define debug_draw_update() debug_draw_update_func() 

// ------------------------------------------------------------------------------------------------

// draw sphere ------------------------------------------------------------------------------------

void debug_draw_draw_circle_func(vec3 plane, vec3 pos, f32 radius, f32* color);

// @DOC: register a sphere to be rendered in debug_draw_update
//       pos:  position of sphere
//       scl:  scale of sphere
//       tint: tint/color of sphere
void debug_draw_sphere_register_func(vec3 pos, float scl, rgbf tint);
// @DOC: register a sphere to be rendered in debug_draw_update using a mat4 instead of vec3 position
//       model: model mat4 of sphere
//       scl:   scale of sphere
//       tint:  tint/color of sphere
void debug_draw_sphere_register_model_func(mat4 model, float scl, rgbf tint);

// ------------------------------------------------------------------------------------------------
// draw line --------------------------------------------------------------------------------------

// @DOC: register a line to be rendered in debug_draw_update
//       uses DEBUG_DEFAULT_LINE_WIDTH for width
//       pos0: start position of line
//       pos1: end position of line
//       tint: tint/color of line
void debug_draw_line_register_func(vec3 pos0, vec3 po1, rgbf tint);
// @DOC: register a line to be rendered in debug_draw_update, also sets line width
//       pos0:  start position of line
//       pos1:  end position of line
//       tint:  tint/color of line
//       width: set the line width
void debug_draw_line_register_width_func(vec3 pos0, vec3 pos1, rgbf tint, f32 width);

// ------------------------------------------------------------------------------------------------
// draw mesh --------------------------------------------------------------------------------------

// @DOC: register a mesh to be rendered in debug_draw_update
//       pos:  position of mesh
//       rot:  rotation of mesh
//       scl:  scale of mesh
//       tint: tint/color of mesh
//       mesh: idx of mesh in assetm
void debug_draw_mesh_register_func(vec3 pos, vec3 rot, vec3 scl, rgbf tint, int mesh);
// @DOC: register a mesh to be rendered in debug_draw_update, using mat4 model instead transform vec3
//       model: mat4 model of mesh
//       tint:  tint/color of mesh
//       mesh:  idx of mesh in assetm
void debug_draw_mesh_register_model_func(mat4 model, rgbf tint, int mesh);
// @DOC: register a mesh to be rendered in debug_draw_update, also uses a texture
//       pos:  position of mesh
//       rot:  rotation of mesh
//       scl:  scale of mesh
//       tint:  tint for texture of mesh
//       mesh:  idx of mesh in assetm
//       tex:   idx of texture in assetm
void debug_draw_mesh_textured_register_func(vec3 pos, vec3 rot, vec3 scl, rgbf tint, int mesh, int tex);
// @DOC: register a mesh to be rendered in debug_draw_update, using mat4 model instead transform vec3, also uses a texture
//       model: mat4 model of mesh
//       tint:  tint for texture of mesh
//       mesh:  idx of mesh in assetm
//       tex:   idx of texture in assetm
void debug_draw_mesh_textured_register_model_func(mat4 model, rgbf tint, int mesh, int tex);

// ------------------------------------------------------------------------------------------------
// draw box ---------------------------------------------------------------------------------------

// @DOC: register a line box / cage to be rendered in debug_draw_update
//       points: array of 8 vec3, defining the corners of the box
//       color:  color of lines
void debug_draw_box_register_func(vec3 points[8], rgbf color);
// @DOC: register a line box / cage to be rendered in debug_draw_update, also define line width
//       points: array of 8 vec3, defining the corners of the box
//       color:  color of lines
//       width:  width of the line
void debug_draw_box_register_width_func(vec3 points[8], rgbf color, f32 width);

// ------------------------------------------------------------------------------------------------

// @NOTE: macros so they can be compiled out 

#define debug_draw_sphere_register(pos, scl, tint)                       debug_draw_sphere_register_func(pos, scl, tint) 
#define debug_draw_sphere_register_model(model, scl, tint)               debug_draw_sphere_register_model_func(model, scl, tint) 

#define debug_draw_line_register(pos0, pos1, tint)                       debug_draw_line_register_func(pos0, pos1, tint)
#define debug_draw_line_register_width(pos0, pos1, tint, width)          debug_draw_line_register_width_func(pos0, pos1, tint, width)

#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)              debug_draw_mesh_register_func(pos, rot, scl, tint, mesh) 
#define debug_draw_mesh_register_model(model, tint, mesh)                debug_draw_mesh_register_model_func(model, tint, mesh) 
#define debug_draw_mesh_textured_register(pos, rot, scl, tint, mesh, tex)        debug_draw_mesh_textured_register_func(pos, rot, scl, tint, mesh, tex)       
#define debug_draw_mesh_textured_register_model(model, tint, mesh, tex)  debug_draw_mesh_textured_register_model_func(model, tint, mesh, tex)       

#define debug_draw_box_register(points, color)                           debug_draw_box_register_func(points, color)
#define debug_draw_box_register_width(points, color, width)              debug_draw_box_register_width_func(points, color, width)


#define debug_draw_draw_circle(plane, pos,  radius, color) debug_draw_draw_circle_func(plane, pos, radius, color)

#else // #ifdef DEBUG_DRAW

// @NOTE: define as nothing to strip out debug code

#define debug_draw_init()   
#define debug_draw_update() 

#define debug_draw_sphere_register(pos, scl, tint)                       
#define debug_draw_sphere_register_model(model, scl, tint)               
#define debug_draw_line_register(pos0, pos1, tint)                       
#define debug_draw_line_register_width(pos0, pos1, tint, width)          
#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)               
#define debug_draw_mesh_register(pos, rot, scl, tint, mesh)              
#define debug_draw_mesh_register_model(model, tint, mesh)                
#define debug_draw_mesh_textured_register(pos, rot, scl, tint, mesh, tex)          
#define debug_draw_mesh_textured_register_model(model, tint, mesh, tex)  
#define debug_draw_box_register(points, color)                           
#define debug_draw_box_register_width(points, color, width)              
#define debug_draw_draw_circle(plane, pos,  radius, color) 

#endif

#endif
