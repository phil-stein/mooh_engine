#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H

#include "global/global.h"

typedef struct timer_t
{
	f64 time;	// time between "timer_start()" and "timer_stop()" in ms
	char* name;	// name of the timer, given in "timer_start()"
	char* file;	// name of the file in which "timer_start()" was called
	// char* func;	// name of the function in which "timer_start()" was called
  int line;
}timer_t;

#ifdef DEBUG_TIMER

// start a timer, pushing a new one onto the stack,
// call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
void   debug_timer_start_timer_func(char* name, const char* file, int line);

// start a timer, pushing a new one onto the stack,
// call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
#define TIMER_START(n) debug_timer_start_timer_func(n, __FILE__, __LINE__)

// check whether there is a timer on the stack to be stopped
bool debug_timer_can_stop_timer();

// pops timer of stack, stopping it, also calculates the "timer.time" value
timer_t  debug_timer_stop_timer_func();
#define TIMER_STOP() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_func()

// stops timer / pops it off stack and prints a message based on the name given in "START_TIMER()"
f64    debug_timer_stop_timer_print_func();
#define TIMER_STOP_PRINT() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_print_func()

// stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
// these timers dont get removed from the array each frame
f64  debug_timer_stop_timer_static_func();
#define TIMER_STOP_STATIC() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_static_func()

// stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
// these timers dont get removed from the array each frame
// prints timers state when stopped
f64  debug_timer_stop_timer_static_print_func();
#define TIMER_STOP_STATIC_PRINT() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_static_print_func()

#define TIMER_FUNC(func)                TIMER_START(#func); func; TIMER_STOP()
#define TIMER_FUNC_PRINT(func)          TIMER_START(#func); func; TIMER_STOP_PRINT()
#define TIMER_FUNC_STATIC(func)         TIMER_START(#func); func; TIMER_STOP_STATIC()
#define TIMER_FUNC_STATIC_PRINT(func)   TIMER_START(#func); func; TIMER_STOP_STATIC_PRINT()

// get the array/stack of current timers
timer_t* debug_timer_get_all(int* len);

// get the array of the static timers
timer_t* debug_timer_get_all_static(int* len);

// !!! only used internally !!!
void debug_timer_clear_state();

#else

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
void debug_timer_start_timer_func(char* name, char* file, char* func);

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_START(n)

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
bool debug_timer_can_stop_timer();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
timer_t  debug_timer_stop_timer_func();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_STOP()

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
f64 debug_timer_stop_timer_print_func();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_STOP_PRINT()

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
timer_t* debug_timer_get_all(int* len);

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
void debug_timer_clear_state();

#define TIMER_FUNC(func)                func
#define TIMER_FUNC_PRINT(func)          func
#define TIMER_FUNC_STATIC(func)         func
#define TIMER_FUNC_STATIC_PRINT(func)   func

#endif

#endif
