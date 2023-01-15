#ifndef RENDERER_H
#define RENDERER_H

#include "global/global.h"
#include "core/types/types.h"
#include "core/terrain.h"
#include "math/math_inc.h"


// start positions of id's in id_buffer
#define ID_BUFFER_ENTITY_0   0    // goes up   0, 1, 2, ...
#define ID_BUFFER_NONE      -1    // nothing selected
#define ID_BUFFER_GIZMO_0   -2    // goes down -2, -3, ...
#define ID_BUFFER_GIZMO_MAX -8    // highest gizmo id allowed, bc terrain 
#define ID_BUFFER_TERRAIN_0 -9    // goes down -9, -10, ...

// turn id from id_buffer <= to ID_BUFFER_TERRAIN_0, 
// into the index for core_data->terrain_chunks
// e.g. -10 - (-9) = -1; -1 * -1 = 1
#define ID_BUFFER_TERRAIN_TO_CHUNK_IDX(id)  (((id) - ID_BUFFER_TERRAIN_0) * -1)

void renderer_init();
void renderer_update();

void renderer_draw_terrain(mat4 view, mat4 proj, terrain_chunk_t* chunk);
void renderer_draw_terrain_mesh(terrain_chunk_t* chunk); // just the draw-call, no shader calls, used for shadow-map

void renderer_draw_scene_mouse_pick(mat4 gizmo_model);
int  renderer_mouse_position_mouse_pick_id();

u32 renderer_gen_brdf_lut();

void renderer_draw_quad(vec3 cam_pos, vec2 pos, vec2 size, vec3 color);

void renderer_draw_quad_textured(vec3 cam_pos, vec2 pos, vec2 size, texture_t* tex);

void renderer_draw_mesh_textured(vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint);
void renderer_draw_mesh_textured_mat(mat4 model, mesh_t* m, texture_t* tex, vec3 tint);

void renderer_draw_mesh_preview(vec3 cam_pos, vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint, texture_t* bg, framebuffer_t* fb);

void renderer_draw_line(vec3 pos0, vec3 pos1, vec3 tint, f32 width);

#endif
