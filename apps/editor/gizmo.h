#ifndef GIZMO_H
#define GIZMO_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"

// @DOC: get the model matrix, and position for the gizmo
//       used in gizmo.c for drawing the gizmo
//       used in renderer.c for drawing the mouse-pick hitbox
//       model is for calc, display_model for rendering
#define GIZMO_MODEL_POS(_app_data, model, display_model, pos)                                 \
    if ((_app_data)->selected_id >= 0)                                                        \
    {                                                                                         \
      entity_t* e = state_entity_get((_app_data)->selected_id);                               \
      mat4_get_pos(e->model, (pos));                                                          \
      mat4_make_model((pos), VEC3(0), VEC3(1), display_model);                                \
      mat4_copy(display_model, model);                                                        \
    }                                                                                         \
    else                                                                                      \
    { mat4_make_identity(model); mat4_make_identity(display_model); vec3_copy(VEC3(0), pos); }

// @NOTE: old, not in use
#define GIZMO_MODEL_POS_OLD(_app_data, model, display_model, pos)                             \
    if ((_app_data)->selected_id >= 0)                                                        \
    {                                                                                         \
      entity_t* e = state_entity_get((_app_data)->selected_id);                               \
      mat4_get_pos(e->model, (pos));                                                          \
      if ((_app_data)->gizmo_type == GIZMO_TRANSLATE)                                         \
      { state_entity_model_no_scale_rotation((_app_data)->selected_id, (model)); }            \
      if ((_app_data)->gizmo_type == GIZMO_ROTATE)                                            \
      { state_entity_model_no_scale((_app_data)->selected_id, (model)); }                     \
      if ((_app_data)->gizmo_type == GIZMO_SCALE)                                             \
      { state_entity_model_no_scale((_app_data)->selected_id, (model)); }                     \
      mat4_make_model((pos), VEC3(0), VEC3(1), display_model);                                \
      mat4_copy(display_model, model);                                                        \
    }                                                                                         \
    else                                                                                      \
    { mat4_make_identity(model); mat4_make_identity(display_model); vec3_copy(VEC3(0), pos); }                                

// @DOC: update the gizmo logic, call once a frame
void gizmo_update();

// @DOC: ends current operation, sets up for next operation
//       also commits an operation to operation.c
void gizmo_end_operation();

// @DOC: calcuate 3d distance between 2 screen-space points projected into and entity's model space
//       p0 & p1: screen-space, in pixel, coordinates
//       pos:     position of gizmo
//       model:   model-space used
//       out:     distance between the projected points
void gizmo_calc_dist_screen_to_model(vec2 p0, vec2 p1, vec3 pos, mat4 model, vec3 out);

#endif
