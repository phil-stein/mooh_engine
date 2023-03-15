#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H

#include "global/global.h"

// @DOC: defines a timer type, used internally
typedef struct timer_t
{
	f64 time;	          // time between "timer_start()" and "timer_stop()" in ms
	char* name;	        // name of the timer, given in "timer_start()"
	char* file;	        // name of the file in which "timer_start()" was called
  int line;           // line where timer was started
  bool  counter_act;
  char* counter_name; // id for counter, counts all timers with same id, -1 for no counter
}timer_t;

// @NOTE: if not defined all this gets stripped out
#ifdef DEBUG_TIMER 

#ifdef DEBUG_TIMER_PRINT
  #define DEBUG_TIMER_PF(...)   PF(__VA_ARGS__)
#else
  #define DEBUG_TIMER_PF(...)  
#endif

// @DOC: inintiallize, call this before any calls to debug_timer / its macros
void debug_timer_init();

  // @DOC: start a timer, pushing a new one onto the stack,
//       call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
//       ! used internally use "TIMER_START()" macro
//       name:        name of timer
//       counter_id:  if timers result should be counted up with other timers set this to be the same on all those timers, otherwise -1
//       file & line: defined in macro, to know call location
void   debug_timer_start_timer_func(char* name, bool counter_act, char* counter_name, const char* file, int line);
// @DOC: start a timer, pushing a new one onto the stack,
//       call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
//       n: name of timer
#define TIMER_START(n) debug_timer_start_timer_func((n), false, "x", __FILE__, __LINE__)
// @DOC: start a timer, pushing a new one onto the stack,
//       call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
//       n:  name of timer
//       id: counter_id for timer, check debug_timer_start_func doc
#define TIMER_START_COUNTER_NAME(n, counter_n) debug_timer_start_timer_func((n), true, (counter_n), __FILE__, __LINE__)
#define TIMER_START_COUNTER(n)                 TIMER_START_COUNTER_NAME((n), (n));

// @DOC: check whether there is a timer on the stack to be stopped
bool debug_timer_can_stop_timer();

// @DOC: pops timer of stack, stopping it, also calculates the "timer.time" value
//       ! used internally use "TIMER_STOP()" macro
timer_t  debug_timer_stop_timer_func(const char* _file, const int _line);
// @DOC: stop timer without printing it, adds it to the static / normal array of timers to be checked
#define TIMER_STOP() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_func(__FILE__, __LINE__)

// @DOC: stops timer / pops it off stack and prints a message based on the name given in "START_TIMER()"
//       ! used internally use "TIMER_STOP_PRINT()" macro
f64    debug_timer_stop_timer_print_func(const char* _file, const int _line);
// @DOC: stop timer and print it, adds it to the static / normal array of timers to be checked
#define TIMER_STOP_PRINT() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_print_func(__FILE__, __LINE__)

// @DOC: stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
//       these timers dont get removed from the array each frame
//       ! used internally use "TIMER_STOP_STATIC()" macro
f64  debug_timer_stop_timer_static_func(const char* _file, const int _line);
// @DOC: stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
//       these timers dont get removed from the array each frame
#define TIMER_STOP_STATIC() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_static_func(__FILE__, __LINE__)

// @DOC: stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
//       these timers dont get removed from the array each frame
//       prints timers state when stopped
//       ! used internally use "TIMER_STOP_STATIC_PRINT()" macro
f64  debug_timer_stop_timer_static_print_func(const char* _file, const int _line);
// @DOC: stops timer and adds it to the array returned by 'debug_timer_get_all_static()'
//       these timers dont get removed from the array each frame
//       prints timers state when stopped
#define TIMER_STOP_STATIC_PRINT() if (!debug_timer_can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } debug_timer_stop_timer_static_print_func(__FILE__, __LINE__)

// @DOC: prints the accumulated times from all timers tagges with counter_id
//       counter_id: id for the timer
void debug_timer_counter_print_func(char* counter_name);
#define TIMER_COUNTER_PRINT(n)   debug_timer_counter_print_func((n))

// @DOC: encapsulate function in timer start and stop, e.g. TIMER_FUNC(func());
#define TIMER_FUNC(func)                TIMER_START(#func); func; TIMER_STOP()
// @DOC: encapsulate function in timer start and stop_print, e.g. TIMER_FUNC_PRINT(func());
#define TIMER_FUNC_PRINT(func)          TIMER_START(#func); func; TIMER_STOP_PRINT()
// @DOC: encapsulate function in timer start and stop_static, e.g. TIMER_FUNC_STATIC(func());
#define TIMER_FUNC_STATIC(func)         TIMER_START(#func); func; TIMER_STOP_STATIC()
// @DOC: encapsulate function in timer start and stop_static_print, e.g. TIMER_FUNC_STATIC_PRINT(func());
#define TIMER_FUNC_STATIC_PRINT(func)   TIMER_START(#func); func; TIMER_STOP_STATIC_PRINT()

// @DOC: get the array/stack of current timers, these get reset each debug_timer_clear_state() call, i.e. each frame
timer_t* debug_timer_get_all(int* len);

// @DOC: get the array of the static timers, these timers dont get cleared
timer_t* debug_timer_get_all_static(int* len);

// @DOC: clear a normal tmiers, should be called each frame in program.c
//       ! used internally
void debug_timer_clear_state();



// @NOTE: strip timer functionality from core
#else // #ifdef DEBUG_TIMER


// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
void debug_timer_init();

void debug_timer_start_timer_func(char* name, bool counter_act, char* counter_name, const char* file, int line);

#define TIMER_START(n)
#define TIMER_START_COUNTER_NAME(n, counter_n)
#define TIMER_START_COUNTER(n)                

bool debug_timer_can_stop_timer();

timer_t  debug_timer_stop_timer_func();

#define TIMER_STOP()

f64 debug_timer_stop_timer_print_func();

#define TIMER_STOP_PRINT()
#define TIMER_STOP_STATIC()
#define TIMER_STOP_STATIC_PRINT()

timer_t* debug_timer_get_all(int* len);

void debug_timer_clear_state();

void debug_timer_counter_print_func(char* counter_name);

#define TIMER_COUNTER_PRINT(n)   

#define TIMER_FUNC(func)                func
#define TIMER_FUNC_PRINT(func)          func
#define TIMER_FUNC_STATIC(func)         func
#define TIMER_FUNC_STATIC_PRINT(func)   func

#endif

#endif
