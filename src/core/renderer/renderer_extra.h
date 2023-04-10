#ifndef CORE_REDERER_EXTRA_H
#define CORE_REDERER_EXTRA_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"

// @TODO: actually use OUTLINE in .c
// @NOTE: so we dont need to define it in make
#ifdef EDITOR
#define OUTLINE
#endif // EDITOR


// @DOC: start positions of id's in id_buffer for mouse_pick
#define ID_BUFFER_ENTITY_0   0    // goes up   0, 1, 2, ...
#define ID_BUFFER_NONE      -1    // nothing selected
#define ID_BUFFER_GIZMO_0   -2    // goes down -2, -3, ...
#define ID_BUFFER_GIZMO_MAX -8    // highest gizmo id allowed, bc terrain 
#define ID_BUFFER_TERRAIN_0 -9    // goes down -9, -10, ...

// @DOC: turn id from id_buffer <= to ID_BUFFER_TERRAIN_0, 
//       into the index for core_data->terrain_chunks
//       e.g. -10 - (-9) = -1; -1 * -1 = 1
#define ID_BUFFER_TERRAIN_TO_CHUNK_IDX(id)  (((id) - ID_BUFFER_TERRAIN_0) * -1)


// @DOC: initialize, call this before any calls to renderer_extra
void renderer_extra_init();

// @DOC: draw the scene with each object having one color representing their id
//       gizmo_model: model matrix of gizmo
void renderer_extra_draw_scene_mouse_pick(mat4 gizmo_model);
// @DOC: get the entity id under the mouse position
//       ! have to have called renderer_extra_draw_scene_mouse_pick() before
int  renderer_extra_mouse_position_mouse_pick_id();

// @DOC: draw the first stage of the outline for the entity core_data->outline_id
//       next/final stage in renderer_update() postfx
void renderer_extra_draw_scene_outline();

// @DOC: generate brdf lut for cubemap
//       ! should be done beforehand and saved as img
//       path: path the bsrdf_lut gets saved to as '.tex'
u32 renderer_extra_gen_brdf_lut(const char* path);


#endif
