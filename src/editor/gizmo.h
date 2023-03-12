#ifndef GIZMO_H
#define GIZMO_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"

// get the model for the gizmo
// used in gizmo.c for drawing the gizmo
// used in renderer.c for drawing the mouse-pick hitbox
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

void gizmo_update();

  // void gizmo_calc_dist(vec2 p0, vec2 p1, vec3 out);
// void gizmo_calc_dist_screen_to_model(vec2 p0, vec2 p1, entity_t* e, vec3 out);
void gizmo_calc_dist_screen_to_model(vec2 p0, vec2 p1, vec3 pos, mat4 model, vec3 out);

#endif
