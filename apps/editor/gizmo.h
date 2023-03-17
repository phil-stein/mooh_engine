#ifndef GIZMO_H
#define GIZMO_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"

// @DOC: get the model matrix, and position for the gizmo
//       used in gizmo.c for drawing the gizmo
//       used in renderer.c for drawing the mouse-pick hitbox
#define GIZMO_MODEL_POS(_app_data, model, pos)                                                \
    if ((_app_data)->selected_id >= 0)                                                        \
    {                                                                                         \
      entity_t* e = state_get_entity((_app_data)->selected_id);                               \
      mat4_get_pos(e->model, (pos));                                                          \
      if ((_app_data)->gizmo_type == GIZMO_TRANSLATE)                                         \
      { state_entity_model_no_scale_rotation((_app_data)->selected_id, (model)); }            \
      if ((_app_data)->gizmo_type == GIZMO_ROTATE)                                            \
      { state_entity_model_no_scale((_app_data)->selected_id, (model)); }                     \
      if ((_app_data)->gizmo_type == GIZMO_SCALE)                                             \
      { state_entity_model_no_scale((_app_data)->selected_id, (model)); }                     \
    }                                                                                         \
    else { mat4_make_identity(model); vec3_copy(VEC3(0), pos); }                                

// @DOC: update the gizmo logic, call once a frame
void gizmo_update();

// @DOC: calcuate 3d distance between 2 screen-space points projected into and entity's model space
//       p0 & p1: screen-space, in pixel, coordinates
//       pos:     position of gizmo
//       model:   model-space used
//       out:     distance between the projected points
void gizmo_calc_dist_screen_to_model(vec2 p0, vec2 p1, vec3 pos, mat4 model, vec3 out);

#endif
