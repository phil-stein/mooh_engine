#include "core/debug/debug_timer.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATOn defined in main.c

#ifdef DEBUG_TIMER

timer_t* timer_stack_arr = NULL;	// stack of all active timers
int timer_stack_arr_len = 0;	    // ^ length

timer_t* cur_state_timer_stack_arr = NULL; // array with the states of the timers popped last frame
int cur_state_timer_stack_arr_len = 0;	   // ^ length
timer_t* state_timer_stack_arr = NULL;     // array with the states of the timers popped last frame
int state_timer_stack_arr_len = 0;	       // ^ length

timer_t* static_timer_stack_arr = NULL; // array with the states of timer that stay once added
int static_timer_stack_arr_len = 0;         // ^ length

struct { char* key; f32 value; }* timer_counters_sh = NULL;  // hashmap with all timer counters
u32 timer_counter_hm_len = 0;

char no_timer_name[] = "no name";

void debug_timer_init()
{
  shdefault(timer_counters_sh, -1.0f);  // set default to be neg
}

void debug_timer_start_timer_func(char* name, bool counter_act, char* counter_name, const char* file, int line)
{
	timer_t t;
	t.time = glfwGetTime();
	t.name = name;
  t.counter_act  = counter_act;
  t.counter_name = counter_name;

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

	arrpush(timer_stack_arr, t);
	timer_stack_arr_len++;
	
  DEBUG_TIMER_PF("started timer | %s | %.2fms | %.2fsec\n -> started \"%s\", line: %d\n", t.name, t.time, t.time * 0.001f, t.file, t.line);
}

bool debug_timer_can_stop_timer()
{
	return timer_stack_arr_len > 0;
}

timer_t debug_timer_stop_timer_func(const char* _file, const int _line)
{
  // return empty if no timer in stack
	if (timer_stack_arr_len <= 0) { timer_t t; t.name = "x"; t.time = 0.0; return t; }

  // remove from arr / stack
	timer_t t = timer_stack_arr[timer_stack_arr_len - 1]; // get last elem // @NOTE: arrpop() does the same
	arrdel(timer_stack_arr, timer_stack_arr_len -1);  // delete last elem
	t.time = glfwGetTime() - t.time; // get the delta t
	t.time *= 1000; // bring to millisecond range
	timer_stack_arr_len--;

  // check for counter, put in arr
  if (t.counter_act)
  {
    // first counter with this id, needs to be added, 0 bc. t.time is added after anyway
    if (shget(timer_counters_sh, t.counter_name) < 0)  
    { shput(timer_counters_sh, t.counter_name, 0.0f); }
   
    // add cur timers time
    f32 total_t = shget(timer_counters_sh, t.counter_name);
    total_t += t.time;
    shput(timer_counters_sh, t.counter_name, total_t);  // key, value
  }

	// timer state
	arrput(cur_state_timer_stack_arr, t);
	cur_state_timer_stack_arr_len++;
	DEBUG_TIMER_PF("stoppped timer | %s | %.2fms | %.2fsec\n -> started \"%s\", line: %d\n -> stopped \"%s\", line: %d\n", t.name, t.time, t.time * 0.001f, t.file, t.line, _file, _line);

	return t;
}

f64  debug_timer_stop_timer_print_func(const char* _file, const int _line)
{
	timer_t t = debug_timer_stop_timer_func(__FILE__, __LINE__);
	PF("[TIMER] | %s | %.2fms | %.2fsec\n -> started \"%s\", line: %d\n -> stopped \"%s\", line: %d\n", t.name, t.time, t.time * 0.001f, t.file, t.line, _file, _line);
	return t.time;
}

f64  debug_timer_stop_timer_static_func(const char* _file, const int _line)
{
	timer_t t = debug_timer_stop_timer_func(__FILE__, __LINE__);
	arrput(static_timer_stack_arr, t);
  static_timer_stack_arr_len++;
  DEBUG_TIMER_PF("stopped static timer | %s |\n -> started \"%s\", line: %d\n -> stopped \"%s\", line: %d\n", t.name, t.file, t.line, _file, _line);
  return t.time;
}

f64  debug_timer_stop_timer_static_print_func(const char* _file, const int _line)
{
	timer_t t = debug_timer_stop_timer_func(__FILE__, __LINE__);
	arrput(static_timer_stack_arr, t);
  static_timer_stack_arr_len++;
	// PF("[TIMER] | %s | %.2fms, %.2fsec\n", t.name, t.time, t.time * 0.001f);
	PF("[TIMER] | %s | %.2fms | %.2fsec\n -> started \"%s\", line: %d\n -> stopped \"%s\", line: %d\n", t.name, t.time, t.time * 0.001f, t.file, t.line, _file, _line);
  return t.time;
}

// prints the counter time for counter_id
void debug_timer_counter_print_func(char* counter_name)
{
  if (shget(timer_counters_sh, counter_name) < 0.0f) 
  { PF("[ERROR TIMER COUNTER] tried stopping timer counter \"%s\", which doesnt exist\n", counter_name); return; }

  f32 time = shget(timer_counters_sh, counter_name);
	PF("[TIMER_COUNTER] | %s | %.2fms | %.2fsec\n", counter_name, time, time * 0.001f);
  // remove from hashmap
  ERR_CHECK(shdel(timer_counters_sh, counter_name) != 0, "timer counters tried deleting non existsing key"); 
}

timer_t* debug_timer_get_all(int* len)
{
	*len = state_timer_stack_arr_len;
	return state_timer_stack_arr;
}

timer_t* debug_timer_get_all_static(int* len)
{
	*len = static_timer_stack_arr_len;
	return static_timer_stack_arr;
}

void debug_timer_clear_state()
{
	// switch last / current frame timer state stack
	timer_t* tmp				    = state_timer_stack_arr;
	int      tmp_len			  = state_timer_stack_arr_len;
	state_timer_stack_arr		    = cur_state_timer_stack_arr;
	state_timer_stack_arr_len	  = cur_state_timer_stack_arr_len;
	cur_state_timer_stack_arr	  = tmp;
	cur_state_timer_stack_arr_len = tmp_len;

	ARRFREE(cur_state_timer_stack_arr);
	cur_state_timer_stack_arr = NULL;
	cur_state_timer_stack_arr_len = 0;
}

#else

void debug_timer_init() { }

void   debug_timer_start_timer_func(char* name, bool counter_act, char* counter_name, const char* file, int line) { }

bool debug_timer_can_stop_timer() { return false; }

timer_t debug_timer_stop_timer_func()
{
	timer_t t; t.time = 0.0; t.name = "x"; t.file = "x"; t.line = -1; return t;
}

f64 debug_timer_stop_timer_print_func() { return 0.0; }

timer_t* debug_timer_get_all_timers(int* len)
{
	*len = 0;
	return NULL;
}

void debug_timer_clear_state() { }

#endif
