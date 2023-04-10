#ifndef GLOBAL_IO_UTIL_H
#define GLOBAL_IO_UTIL_H

// @NOTE: need to define IO_UTIL_IMPLEMENTATION once before including 
//        #define IO_UTIL_IMPLEMENTATION
//        #include "io_util.h"


// @DOC: get width and height of current console
//       w: gets set to console width
//       h: gets set to console height
void io_util_get_console_size_win(int* w, int* h);


#endif

// @DOC: need to define this once before including 
#ifdef IO_UTIL_IMPLEMENTATION

#include <windows.h>

void io_util_get_console_size_win(int* w, int* h)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  *w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#endif
