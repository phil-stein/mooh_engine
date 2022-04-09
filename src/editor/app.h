#ifndef GAME_H
#define GAME_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"

typedef enum gizmo_type_t { GIZMO_NONE, GIZMO_TRANSLATE, GIZMO_SCALE, GIZMO_ROTATE } gizmo_type_t;

typedef enum terrain_edit_type_t 
{ 
  TERRAIN_EDIT_NONE, 
  TERRAIN_EDIT_SCULPT, 
  TERRAIN_EDIT_LEVEL, 
  TERRAIN_EDIT_SMOOTH, 
  TERRAIN_EDIT_PAINT 

} terrain_edit_type_t;

typedef struct app_data_t
{
  int selected_id;
  
  gizmo_type_t gizmo_type;
  bool  switch_gizmos_act;     // if false hotkeys wont switch gizmo
  float gizmo_translate_speed;
  float gizmo_rotate_speed;
  float gizmo_scale_speed;
  bool  gizmo_snapping;
  float gizmo_translate_snap;
  float gizmo_rotate_snap;
  float gizmo_scale_snap;

  float mouse_sensitivity;
  bool wireframe_act;
  bool mouse_over_ui;

  terrain_edit_type_t terrain_edit_type;
  f32  terrain_edit_radius;
  f32  terrain_edit_strength;
  const f32  terrain_edit_scalar;
  const f32  terrain_edit_smooth_scalar;
  int terrain_edit_paint_material;
  const f32  terrain_edit_paint_scalar;

}app_data_t;

#define APP_DATA_INIT()                                \
{                                                      \
  .selected_id                  = -1,                  \
                                                       \
  .gizmo_type                   = GIZMO_NONE,          \
  .switch_gizmos_act            = true,                \
  .gizmo_translate_speed        = 1.0f,                \
  .gizmo_rotate_speed           = 20.0f,               \
  .gizmo_scale_speed            = 1.0f,                \
  .gizmo_snapping               = false,               \
  .gizmo_translate_snap         = 1.0f,                \
  .gizmo_rotate_snap            = 15.0f,               \
  .gizmo_scale_snap             = 0.5f,                \
                                                       \
  .mouse_sensitivity            = 0.125f,              \
  .wireframe_act                = false,               \
  .mouse_over_ui                = false,               \
                                                       \
  .terrain_edit_type            = TERRAIN_EDIT_NONE,   \
  .terrain_edit_radius          = 5.0f,                \
  .terrain_edit_scalar          = 0.01f,               \
  .terrain_edit_smooth_scalar   = 0.5f,                \
  .terrain_edit_strength        = 1.0,                 \
  .terrain_edit_paint_material  = 0,                   \
  .terrain_edit_paint_scalar    = 0.75f,               \
}


// -- keymappings --

#define KEY_EXIT              KEY_ESCAPE

#define KEY_WIREFRAME_TOGGLE  KEY_TAB

#define KEY_GIZMO_TRANSLATE   KEY_G
#define KEY_GIZMO_SCALE       KEY_S
#define KEY_GIZMO_ROTATE      KEY_R
#define KEY_GIZMO_SNAPPING    KEY_LEFT_CONTROL


#define CAM_SPEED             5
#define CAM_SPEED_SHIFT_MULT  5
#define KEY_MOUSE_MOVE_START  MOUSE_RIGHT
#define KEY_MOVE_FORWARD      KEY_W 
#define KEY_MOVE_BACKWARD     KEY_S
#define KEY_MOVE_LEFT         KEY_A
#define KEY_MOVE_RIGHT        KEY_D 
#define KEY_MOVE_UP           KEY_E 
#define KEY_MOVE_DOWN         KEY_Q 


#define SCENE_FILE_NAME   "test.scene"
#define TERRAIN_FILE_NAME "test.terrain"

void app_init();
void app_update();

app_data_t* app_data_get();


#endif
