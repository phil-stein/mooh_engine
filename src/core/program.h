#ifndef CORE_PROGRAM_H
#define CORE_PROGRAM_H

#include "global/global.h"
#include "core/window.h"


// -- keymappings --
// app specific inn app.h
#define KEY_EXIT              KEY_ESCAPE
#define KEY_WIREFRAME_TOGGLE  KEY_TAB
#define KEY_TOGGLE_FULLSCREEN KEY_F11


// @DOC: function that starts and runs the entire program
//       set refresh rate to <= 0 to choose native monitor refresh rate
//       width:      pixel width of window buffer
//       width:      pixel height of window buffer
//       title:      title of window
//       type:       min, max, fullscreen see window.h
//       init_f:     gets called once at startup 
//       update_f:   gets called once each frame
//       asset_path: path to the root asset folder
// void program_start(int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f, const char* asset_path);
void DBG(program_start_dbg, int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f, const char* asset_path);
#define program_start(...)  program_start_dbg(__VA_ARGS__ DBG_F_L)

// @DOC: syncs entity and phys_obj positions after the phys simulation ran
//       ! not sure if to move this to state
void program_sync_phys();

// @DOC: quits program, duh
void program_quit(); 

// @DOC: logic commmon/shared between most applications
INLINE void programm_app_default_logic(core_data_t* core_data)
{

 
  // toggle phys debug display, only works in play_mode as it happens in program_phys_sync
  if (input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_TAB) && core_data_is_play())
  { core_data->phys_debug_act = !core_data->phys_debug_act; }

  // toggle shadows
  if (input_get_key_pressed(KEY_SPACE))
  { core_data->show_shadows = !core_data->show_shadows; }
 
  // toggle wireframe, ctrl+tab is toggle phys display
  if (input_get_key_pressed(KEY_WIREFRAME_TOGGLE) && !input_get_key_down(KEY_LEFT_CONTROL))
	{
		core_data->wireframe_mode_enabled = !core_data->wireframe_mode_enabled;
		// core_data->wireframe_mode_enabled = app_data.wireframe_act;
	  // app_data.wireframe_act = !app_data.wireframe_act;
	}

  if (input_get_key_pressed(KEY_TOGGLE_FULLSCREEN))
  {
    window_type type = window_get_mode();
    
    P_WINDOW_TYPE(type); 
    
    // @NOTE: min -> max -> full
    type = type == WINDOW_MIN ? WINDOW_MAX : type == WINDOW_MAX ? WINDOW_FULL : WINDOW_MAX;
    
    P_WINDOW_TYPE(type); 
    P("------------");

    window_set_mode(type);
  }

  if (input_get_key_pressed(KEY_EXIT))
  {
#ifdef EDITOR
    if (core_data->scripts_act || core_data->phys_act)
    { core_data_pause(); } // { core_data->scripts_act = false;  core_data->phys_act = false; }
    else { program_quit(); }
#else
    program_quit();
#endif
  }
}

#endif 
