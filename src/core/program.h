#ifndef CORE_PROGRAM_H
#define CORE_PROGRAM_H

#include "global/global.h"
#include "core/window.h"


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


#endif 
