#ifndef RENDERER_DIRECT_H
#define RENDERER_DIRECT_H

#include "../../global/global.h"
#include "../../core/types/types.h"
#include "../../math/math_inc.h"


void renderer_direct_init();

void renderer_direct_draw_quad(vec3 cam_pos, vec2 pos, vec2 size, vec3 color);

void renderer_direct_draw_quad_textured(vec3 cam_pos, vec2 pos, vec2 size, texture_t* tex);

void renderer_direct_draw_mesh_textured(vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint);
void renderer_direct_draw_mesh_textured_mat(mat4 model, mesh_t* m, texture_t* tex, vec3 tint);

void renderer_direct_draw_mesh_preview(vec3 cam_pos, vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint, texture_t* bg, framebuffer_t* fb);

void renderer_direct_draw_line(vec3 pos0, vec3 pos1, vec3 tint, f32 width);


#endif
