#ifndef RENDERER_H
#define RENDERER_H

#include "global/global.h"
#include "core/types/types.h"
#include "core/terrain.h"
#include "math/math_inc.h"

// @NOTE: so we dont need to define it in make
#ifdef EDITOR
#define OUTLINE
#endif // EDITOR


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

void renderer_draw_scene_outline();

u32 renderer_gen_brdf_lut();

#endif
