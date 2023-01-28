#ifndef PROGRAM_H
#define PROGRAM_H

#include "global/global.h"
#include "core/window.h"


// function that starts and runs the entire program
// set refresh rate to <= 0 to choose native monitor refresh rate
// init_f & update_f get called once at the start and each frame respectively
void program_start(int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f, const char* asset_path);

void program_sync_phys();

void program_quit(); 


#endif 
