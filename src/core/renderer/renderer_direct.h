#ifndef CORE_RENDERER_DIRECT_H
#define CORE_RENDERER_DIRECT_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"


// @DOC: initialize, call this before any other calls to renderer_direct
void renderer_direct_init();

// @DOC: draw a colored quad
//       cam_pos: position of camera
//       pos:     position of quad
//       size:    size of quad
//       color:   color of quad
// void renderer_direct_draw_quad(vec3 cam_pos, vec2 pos, vec2 size, vec3 color);
void renderer_direct_draw_quad(vec2 cam_pos, f32 cam_zoom, vec2 pos, vec2 size, vec3 color);

// @DOC: draw a textured quad
//       cam_pos: position of camera
//       pos:     position of quad
//       size:    size of quad
//       tex:     texture to be drawn on quad
// void renderer_direct_draw_quad_textured(vec3 cam_pos, vec2 pos, vec2 size, texture_t* tex);
void renderer_direct_draw_quad_textured(vec2 cam_pos, f32 cam_zoom, vec2 pos, vec2 size, texture_t* tex, rgbf tint);

// @DOC: draw a textured mesh
//       pos:   position
//       rot:   rotation
//       scale: scale
//       m:     mesh_t to be drawn
//       tex:   texture to be drawn on mesh
//       tint:  tint the texture with a color
void renderer_direct_draw_mesh_textured(vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint);
// @DOC: draw a textured mesh, using a model matrix
//       model: model matrix 
//       m:     mesh_t to be drawn
//       tex:   texture to be drawn on mesh
//       tint:  tint the texture with a color
void renderer_direct_draw_mesh_textured_mat(mat4 model, mesh_t* m, texture_t* tex, vec3 tint);

// @DOC: draw a textured mesh, into a framebuffer, i.e. for preview of mesh in gui or inventory icons
//       cam_pos: position of camera
//       pos:   position
//       rot:   rotation
//       scale: scale
//       m:     mesh_t to be drawn
//       tex:   texture to be drawn on mesh
//       tint:  tint the texture with a color
//       bg:    texture to be drawn in the background
//       fb:    framebuffer to be rendered into, has to be initialized
void renderer_direct_draw_mesh_preview(vec3 cam_pos, vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint, texture_t* bg, framebuffer_t* fb);

// @DOC: draw a colored line
//       pos0:  start position of line
//       pos1:  end position of line
//       tint:  color of line
//       width: width of line
void renderer_direct_draw_line(vec3 pos0, vec3 pos1, vec3 tint, f32 width);


#endif
