#ifndef IO_UTIL_H
#define IO_UTIL_H

void io_util_get_console_size_win(int* w, int* h);


#endif

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
