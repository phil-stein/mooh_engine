#ifndef APP_H
#define APP_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"

// @DOC: the different gizmo types / modes
typedef enum gizmo_type_t { GIZMO_NONE, GIZMO_TRANSLATE, GIZMO_SCALE, GIZMO_ROTATE } gizmo_type_t;

// @DOC: the different types / modes for the terrain edit tool
typedef enum terrain_edit_type_t 
{ 
  TERRAIN_EDIT_NONE, 
  TERRAIN_EDIT_SCULPT, 
  TERRAIN_EDIT_LEVEL, 
  TERRAIN_EDIT_SMOOTH, 
  TERRAIN_EDIT_PAINT 

} terrain_edit_type_t;

#define GUI_INFO_STR_MAX 64

// @DOC: houses all publicly accessible data for app (editor)
//       instance of app_data_t is in app.c and accesible via app_data_get()
typedef struct app_data_t
{
  int selected_id;                        // id of current selected entity, -1 is none 
  
  gizmo_type_t gizmo_type;                // current active gizmo type / mode
  bool  switch_gizmos_act;                // if false hotkeys wont switch gizmo
  float gizmo_translate_speed;            // speed at which GIZMO_TRANSLATE moves ents
  float gizmo_rotate_speed;               // speed at which GIZMO_ROTATE rotates ents
  float gizmo_scale_speed;                // speed at which GIZMO_SCALE scales ents
  bool  gizmo_snapping;                   // if true gizmo movec, rotates, scales in set intervals
  float gizmo_translate_snap;             // interval at which entities are moved if gizmo_snapping is true
  float gizmo_rotate_snap;                // interval at which entities are rotatet if gizmo_snapping is true
  float gizmo_scale_snap;                 // interval at which entities are scaled if gizmo_snapping is true

  char gui_info_str[GUI_INFO_STR_MAX];    // info text in top bar
  f32  gui_info_t;                        // time remaining for info text in top bar to be displayed

  float mouse_sensitivity;                // speed at which the mouse moves
  bool  mouse_over_ui;                     // if true mouse is currently over a ui window

  terrain_edit_type_t terrain_edit_type;  // current active terrain edit tool type / mode 
  f32  terrain_edit_radius;               // radius in which the terrain edit tool has effect
  f32  terrain_edit_strength;             // effect the current terrain edit tool has
  const f32  terrain_edit_scalar;         // multiplier for terrain_edit_strengh
  const f32  terrain_edit_smooth_scalar;  // multiplier for terrain_edit_strengh
  int terrain_edit_paint_material;        // multiplier for terrain_edit_strengh 
  const f32  terrain_edit_paint_scalar;   // multiplier for terrain_edit_strengh 
    
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
  .gui_info_str                 = "",                  \
  .gui_info_t                   = 0.0f,                \
                                                       \
  .mouse_sensitivity            = 0.125f,              \
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
  // .wireframe_act                = false,               \x

#define GUI_INFO_DEFAULT_T  2.0f
#define GUI_INFO_STR_SET_T(_app_data, t, ...)   SPRINTF(GUI_INFO_STR_MAX, (_app_data)->gui_info_str, __VA_ARGS__); \
                                                (_app_data)->gui_info_t = t
#define GUI_INFO_STR_SET(_app_data, ...)        GUI_INFO_STR_SET_T(_app_data, GUI_INFO_DEFAULT_T, __VA_ARGS__)

// -- keymappings --
// some are in program.h

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

// @DOC: initalize, call before any other calls to app
void app_init();
// @DOC: upate logic called once a frame
void app_update();

// @DOC: returns a pointer to app_data_t var in app.c
app_data_t* app_data_get();

void app_entity_removed_callback(int id);


#endif
