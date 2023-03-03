#include "core/debug/debug_timer.h"
#include "core/str_util.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATOn defined in main.c

#ifdef DEBUG_TIMER

timer_t* timer_stack = NULL;	// stack of all active timers
int timer_stack_len = 0;	    // ^ length

timer_t* cur_state_timer_stack = NULL; // array with the states of the timers popped last frame
int cur_state_timer_stack_len = 0;	   // ^ length
timer_t* state_timer_stack = NULL;     // array with the states of the timers popped last frame
int state_timer_stack_len = 0;	       // ^ length

timer_t* static_timer_stack = NULL; // array with the states of timer that stay once added
int static_timer_stack_len = 0;         // ^ length

char no_timer_name[] = "no name";

void debug_timer_start_timer_func(char* name, const char* file, int line)
{
	timer_t t;
	t.time = glfwGetTime();
	t.name = name;

	char* file_name = str_util_find_last_of((char*)file, "\\");
  if (file_name == NULL || strlen(file_name) == 0)
  {
	  file_name = str_util_find_last_of((char*)file, "/");
  }
  if (file_name == NULL || strlen(file_name) == 0)
  {
    if (strlen(file) > 0) { file_name = (char*)file; }
    else { file_name = no_timer_name; }
  }
  else { file_name++; } // the first char is the '\'
  t.file = file_name;

  t.line = line;
	// t.func = func;

	arrpush(timer_stack, t);
	timer_stack_len++;
}

bool debug_timer_can_stop_timer()
{
	return timer_stack_len > 0;
}

timer_t debug_timer_stop_timer_func()
{
	if (timer_stack_len <= 0) { timer_t t; t.name = "x"; t.time = 0.0; return t; }

	timer_t t = timer_stack[timer_stack_len - 1]; // get last elem
	arrdel(timer_stack, timer_stack_len -1);  // delete last elem
	t.time = glfwGetTime() - t.time; // get the delta t
	t.time *= 1000; // bring to millisecond range
	timer_stack_len--;

	// timer state
	arrput(cur_state_timer_stack, t);
	cur_state_timer_stack_len++;

	return t;
}

f64  debug_timer_stop_timer_print_func()
{
	timer_t t = debug_timer_stop_timer_func();
	printf("[TIMER] | %s | %.2fms, %.2fsec\n", t.name, t.time, t.time * 0.001f);
	return t.time;
}

f64  debug_timer_stop_timer_static_func()
{
	timer_t t = debug_timer_stop_timer_func();
	arrput(static_timer_stack, t);
  static_timer_stack_len++;
  return t.time;
}

f64  debug_timer_stop_timer_static_print_func()
{
	timer_t t = debug_timer_stop_timer_func();
	arrput(static_timer_stack, t);
  static_timer_stack_len++;
	printf("[TIMER] | %s | %.2fms, %.2fsec\n", t.name, t.time, t.time * 0.001f);
  return t.time;
}

timer_t* debug_timer_get_all(int* len)
{
	*len = state_timer_stack_len;
	return state_timer_stack;
}

timer_t* debug_timer_get_all_static(int* len)
{
	*len = static_timer_stack_len;
	return static_timer_stack;
}

void debug_timer_clear_state()
{
	// switch last / current frame timer state stack
	timer_t* tmp				    = state_timer_stack;
	int      tmp_len			  = state_timer_stack_len;
	state_timer_stack		    = cur_state_timer_stack;
	state_timer_stack_len	  = cur_state_timer_stack_len;
	cur_state_timer_stack	  = tmp;
	cur_state_timer_stack_len = tmp_len;

	arrfree(cur_state_timer_stack);
	cur_state_timer_stack = NULL;
	cur_state_timer_stack_len = 0;
}

#else

void debug_timer_start_timer_func(char* name, char* file, char* func)
{ }

bool debug_timer_can_stop_timer()
{ return false; }

timer_t debug_timer_stop_timer_func()
{
	timer_t t; t.time = 0.0; t.name = "x"; t.file = "x"; t.line = -1; return t;
}

f64 debug_timer_stop_timer_print_func()
{ return 0.0; }

timer_t* debug_timer_get_all_timers(int* len)
{
	*len = 0;
	return NULL;
}

void debug_timer_clear_state()
{ }

#endif
