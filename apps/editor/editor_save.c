#include "editor/editor_save.h"
#include "editor/app.h"
#include "core/core_data.h"
// #include "core/state.h"
// #include "core/io/assetm.h"
#include "core/event_sys.h"
#include "core/io/file_io.h"
#include "core/debug/debug_timer.h"
#include "serialization/serialization.h"

#include "stb/stb_ds.h"

static core_data_t* core_data = NULL;
static app_data_t*  app_data  = NULL;

int selected_id = -1;
int outline_id  = -1;

void editor_save_play_state_callback(bool state)
{
  if (state)
  {
    editor_save_info();
    // app_data->selected_id = -1; // so properties stay opeg
    core_data->outline_id = -1;
  }
  else
  {
    editor_load_info();
  }
  P_ERR("play state is: %s\n", STR_BOOL(state));
}
void editor_save_init()
{
  core_data = core_data_get();
  app_data  = app_data_get();
  
  event_sys_register_play_state(editor_save_play_state_callback);
}

void editor_save_info()
{
  selected_id = app_data->selected_id;
  outline_id  = core_data->outline_id;
}
void editor_load_info()
{
  app_data->selected_id = selected_id;
  core_data->outline_id = outline_id;
}

// void editor_save_write_info_to_file(const char* name)
// {
//   core_data_t* core_data = core_data_get();
//   app_data_t*  app_data  = app_data_get();
// 
//   u8* buffer = NULL;
//   
//   // selected entity
//   serialization_serialize_s32(buffer, app_data->selected_id);
//   // outline entity
//   serialization_serialize_s32(buffer, core_data->outline_id);
//   
//   // windows min/max -> close all during play
// 
//   // ...
// 
//   char path[ASSET_PATH_MAX +64];
//   SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s%s%s", core_data->asset_path, "/editor/", EDITOR_SAVE_NAME, EDITOR_SAVE_EXTENSION);
//   file_io_write(path, (const char*)buffer, (int)arrlen(buffer));
// 
//   ARRFREE(buffer);
// }

