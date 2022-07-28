#ifndef GAME_H
#define GAME_H

#include "global/global.h"
#include "core/input.h"
#include "core/types/types.h"

typedef struct app_data_t
{

  float mouse_sensitivity;
  bool mouse_over_ui;

}app_data_t;

#define APP_DATA_INIT()                                \
{                                                      \
  .mouse_sensitivity            = 0.125f,              \
  .mouse_over_ui                = false,               \
}


// -- keymappings --

#define KEY_EXIT              KEY_ESCAPE

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
