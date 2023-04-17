#ifndef GLOABL_GLOBAL_H
#define GLOABL_GLOBAL_H


// include all files like this once and the only use global.h
// // order is important, io_util & str_util before global
// #define IO_UTIL_IMPLEMENTATION       // only define once
// #include "global/io_util.h"          // only need to include here, normally included via global.h
// #define STR_UTIL_IMPLEMENTATION      // only define once
// #include "global/str_util.h"         // only need to include here, normally included via global.h
// #define GLOBAL_BOOL_TYPE int/u8/etc. // optional is int by default
// #include "global/global.h"


// util headers in this repo
#include "io_util.h"  // needs  IO_UTIL_IMPLEMENTATION defined ONCE
#include "str_util.h" // needs STR_UTIL_IMPLEMENTATION defined ONCE

// libs needed basically everywhere
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>



// -- null --
#ifndef NULL
	#define NULL (void*)0
#endif

// -- bool --
#ifndef GLOBAL_BOOL_TYPE
  #define GLOBAL_BOOL_TYPE int
#endif

#ifndef bool
	#define bool GLOBAL_BOOL_TYPE
#endif
#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif


// ---- typedefs ----

typedef char			          s8;
typedef short			          s16;
typedef int			            s32;
typedef long long		        s64;

typedef unsigned char		    u8;
typedef unsigned short		  u16;
typedef unsigned int 		    u32;
typedef unsigned long	long  u64;

typedef float 			        f32;
typedef double			        f64;

typedef void (empty_callback)(void);

// @DOC: inlining works different in msvc and gcc
#if defined(_MSC_VER)
#  define INLINE __forceinline
#else
#  define INLINE static inline __attribute((always_inline))
#endif

// ---- helper ----

// @DOC: make number with bit a set 
//       1 << 0 0000
//       1 << 1 0001
//       1 << 2 0010
//       1 << 3 0100
//       1 << 4 1000
//       aka. 1<<a == 2^a, 0, 2, 4, 8, ...
#define FLAG(a) (1 << (a)) 

// @DOC: check if flag a and b have a set bit in common
//       a: 00010010
//       b: 00000010
//       &: 00000010 true bc. >1
#define HAS_FLAG(a, b)   ((a) & (b))

// @DOC: set all shared bits between a and b to 0 in a 
#define REMOVE_FLAG(a, b) ((a) &= ~(b))


// @DOC: turn macro input to string
#define STR_VAR(v)  (#v)
// @DOC: turn bool to string
#define STR_BOOL(v) ((v) ? "true" : "false")

// @DOC: ifdef activates P... macros, ASSERT, ERR..., etc.
// #define GLOBAL_DEBUG
#ifdef GLOBAL_DEBUG

// -- style --

// @DOC: used for setting terminal output to a specific mode, using PF_MODE(), PF_STYLE, etc.
typedef enum pf_mode
{
  PF_NORMAL     = 0,
  PF_BOLD       = 1,    // bright
  PF_DIM        = 2,
  PF_ITALIC     = 3,
  PF_UNDERLINE  = 4,
  PF_REVERSE    = 5,
  PF_HIDDEN     = 6
}pf_mode;
// @DOC: used for setting terminal output to a specific text color, using PF_MODE(), PF_STYLE, etc.
typedef enum pf_fg
{
  PF_BLACK    = 30,
  PF_RED      = 31,
  PF_GREEN    = 32,
  PF_YELLOW   = 33,
  PF_BLUE     = 34,
  PF_PURPLE   = 35,
  PF_CYAN     = 36,
  PF_WHITE    = 37 
}pf_color;
// @DOC: used for setting terminal output to a specific background color, using PF_MODE(), PF_STYLE, etc.
typedef enum pf_bg
{
  PF_BG_BLACK    = 40,
  PF_BG_RED      = 41,
  PF_BG_GREEN    = 42,
  PF_BG_YELLOW   = 43,
  PF_BG_BLUE     = 44,
  PF_BG_PURPLE   = 45,
  PF_BG_CYAN     = 46,
  PF_BG_WHITE    = 47 
}pf_bg;

// @DOC: doesnt print location, just printf
#define _PF(...)		printf(__VA_ARGS__)

// @DOC: setting terminal output to a specific mode, text and background color
#define PF_MODE(style, fg, bg)   _PF("\033[%d;%d;%dm", style, fg, bg)
// @DOC: setting terminal output to a specific mode and text color
#define PF_STYLE(style, color)   _PF("\033[%d;%dm", style, color)
// @DOC: setting terminal output to a specific text color
#define PF_COLOR(color)          PF_STYLE(PF_NORMAL, color)
// @DOC: setting terminal output to default mode, text and background color
#define PF_MODE_RESET()          PF_MODE(PF_NORMAL, PF_WHITE, PF_BG_BLACK)
// @DOC: setting terminal output to default mode and text
#define PF_STYLE_RESET()         PF_STYLE(PF_NORMAL, PF_WHITE)

// -- print --

// @DOC: print location, as in file and line, append anywhere that info is usefull
#define P_LOCATION() PF_STYLE(PF_DIM, PF_WHITE); PF_STYLE(PF_ITALIC, PF_WHITE); _PF(" -> file: %s, line: %d\n", __FILE__, __LINE__); PF_STYLE_RESET()
// @DOC: print location on all P_ macros or not
// #define PF_PRINT_LOCATION
#ifdef PF_PRINT_LOCATION
  #define PF_IF_LOC() P_LOCATION() 
#else
  #define PF_IF_LOC()
#endif

#define PF(...)		  printf(__VA_ARGS__); PF_IF_LOC()                                                      // @DOC: printf
#define P(msg)		  _PF("%s\n", msg); PF_IF_LOC()                                                         // @DOC: pritnf with automatic \n
#define P_INFO(msg) PF_COLOR(PF_YELLOW); _PF("[INFO] "); PF_STYLE_RESET(); _PF("%s\n", msg); P_LOCATION() // @DOC: P(), but always prints location

// @DOC: draw --- line as long as the current console is wide, only works on windows
#if defined( _WIN32 )
#define P_LINE()    { int w, h; io_util_get_console_size_win(&w, &h); for (int i = 0; i < w -1; ++i) { _PF("-"); } _PF("\n"); }
#else
#define P_LINE()    P_ERR("P_LINE() currently only supported in windows")
#endif

// -- print variables --


// @DOC: print the different types, e.g. P_INT(variable), highlights variable name cyan
#define P_SIGNED(v) 	PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": %d\n", (v)); PF_IF_LOC()
#define P_INT(v) 	    P_SIGNED(v)
#define P_S32(v) 	    P_SIGNED(v) 
#define P_S16(v) 	    P_SIGNED(v) 
#define P_S8(v) 	    P_SIGNED(v) 
#define P_UNSIGNED(v) PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": %u\n", (v)); PF_IF_LOC()
#define P_U64(u)      printf("|%s| %"PRId64"\n", #u, u)
#define P_U32(v)      P_UNSIGNED(v)
#define P_U16(v)      P_UNSIGNED(v)
#define P_U8(v)       P_UNSIGNED(v)

#define P_F32(v) 	    PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": %f\n", (v)); PF_IF_LOC()
// #define P_U64(u)   printf("|%s| %llu\n", #u, u)

#define P_BOOL(v) 	  PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": %s\n", STR_BOOL(v)); PF_IF_LOC()

#define P_CHAR(v) 	  PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": '%c'\n", (char)(v)); PF_IF_LOC()
#define P_STR(v) 	    PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": \"%s\"\n", (v)); PF_IF_LOC() 
#define P_TXT(v)      PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(":\n%s\n", (v)); PF_IF_LOC()    

#define P_PTR(v) 	    PF_COLOR(PF_CYAN); _PF("%s", #v); PF_STYLE_RESET(); _PF(": %p\n", (v)); PF_IF_LOC()

// always print location

#define P_LOC_INT(v)  P_INT(v);  P_LOCATION()
#define P_LOC_S32(v) 	P_S32(v);  P_LOCATION() 
#define P_LOC_S16(v) 	P_S16(v);  P_LOCATION() 
#define P_LOC_S8(v) 	P_S8(v);   P_LOCATION()
#define P_LOC_U8(v)   P_U8(v);   P_LOCATION()
#define P_LOC_U32(v)  P_U32(v);  P_LOCATION()
#define P_LOC_U16(v)  P_U16(v);  P_LOCATION() 
#define P_LOC_F32(v) 	P_F32(v);  P_LOCATION()
#define P_LOC_BOOL(v) P_BOOL(v); P_LOCATION()
#define P_LOC_CHAR(v) P_CHAR(v); P_LOCATION()
#define P_LOC_STR(v) 	P_STR(v);  P_LOCATION() 
#define P_LOC_TXT(v)  P_TXT(v);  P_LOCATION()    
#define P_LOC_PTR(v)  P_PTR(v);  P_LOCATION()

// -- debug --

// @DOC: print an error with location, without stopping the execution
#define P_ERR(...)	PF_COLOR(PF_RED); _PF("[ERROR] "); PF_MODE_RESET(); _PF(__VA_ARGS__); P_LOCATION()
// @DOC: print an error with location if the condition c if false, stopping the execution
#define ASSERT(c)   if(!(c)) { PF_COLOR(PF_RED); _PF("[ASSERT]" ); PF_MODE_RESET(); _PF("'%s'\n", #c); P_LOCATION(); abort(); }
// @DOC: print an error with location, stopping the execution
#define ERR(...)  P_ERR(__VA_ARGS__); abort();
// @DOC: print an error with location, and custom message if the condition c if false, stopping the execution
#define ERR_CHECK(c, ...) if(!(c)) { ERR(__VA_ARGS__); }
// @DOC: print an error with location, and custom message if the condition c if false, without stopping the execution
#define P_ERR_CHECK(c, ...) if(!(c)) { P_ERR(__VA_ARGS__); }

// @DOC: check f32/float is nan
#define F32_NAN(v)  (isnan(v) != 0)
// @DOC: print if f32/float is nan
#define P_NAN(v)    { if (F32_NAN(v)) { _PF("%s is nan\n", #v); PF_IF_LOC(); } }

// @DOC: print out all macros once for testing
#define GLOBAL_TEST_P_MACROS() { s32 int_32 = 0; s16 int_16 = 0; s8 int_8 = 0; u32 uint_32 = 0; u16 uint_16 = 0; u8 uint_8  = 0; f32 floating_point = 0.5f; bool b = true; s8 s_byte = '?'; char* str = "hello, there"; char* txt = "this is very textual\nmhhh yess\nlicrictically intricate"; \
P_INT(int_32); P_S32(int_32); P_S16(int_16); P_S8(int_8); P_U32(uint_32); P_U16(uint_16); P_U8(uint_8); P_F32(floating_point); P_BOOL(b); P_CHAR(s_byte); P_STR(str); P_TXT(txt); P_INFO("info"); P_ERR("hello\n"); /*ASSERT(0);*/ }


// @DOC: print the currently in use version of c
#define P_C_VERSION()                     \
  if (__STDC_VERSION__ >=  201710L)       \
  { PF("-- using c18 --\n"); }            \
  else if (__STDC_VERSION__ >= 201112L)   \
  { PF("-- using c11 --\n"); }            \
  else if (__STDC_VERSION__ >= 199901L)   \
  { PF("-- using c99 --\n"); }            \
  else                                    \
  { PF("-- using c89/c90 --\n"); }

// -- func wrapper --

// @DOC: wrappers around standard functions to make the safer, can be compiled out

#define SPRINTF(max, ...)        ASSERT(sprintf(__VA_ARGS__) < (max))                   
#define STRCPY(dest, source)     ASSERT(strcpy(dest, source) != NULL)

#define MALLOC(ptr, size)        (ptr) = malloc(size);        ASSERT((ptr) != NULL)     
#define CALLOC(ptr, items, size) (ptr) = calloc(items, size); ASSERT((ptr) != NULL)     
#define REALLOC(ptr, size)       (ptr) = realloc(ptr, size);  ASSERT((ptr) != NULL)     
#define FREE(ptr)                ASSERT(ptr != NULL); free(ptr)                         

// stb_ds
#define ARRFREE(a)               arrfree((a)); (a) = NULL  
#define HMFREE(a)                hmfree((a));  (a) = NULL
#define SHFREE(a)                shfree((a));  (a) = NULL

// -- debug func --

// @UNSURE:
// @DOC: wraps a function with file and line args
//       void DBG(func(int arg))
//       ->
//       void func(int arg, const char* _file, const int _line)
//       this way it can be compiled out
#define DBG(func, ...)       (func)(__VA_ARGS__, const char* _file, const int _line)
// @DOC: file and line macro for defining DBG() funcs macros
//       void DBG(func(int arg))
//       #define func(arg)      func(arg DBG_F_L)
//       this way it can be compiled out
#define DBG_F_L              , __FILE__, __LINE__

#else // GLOBAL_DEBUG --------------------------------------------------------------------------------------

// @DOC: compile out all GLOBAL_DEBUG macros

#define _PF(...)		
#define PF_MODE(style, fg, bg)   
#define PF_STYLE(style, color)   
#define PF_COLOR(color)          
#define PF_MODE_RESET()          
#define PF_STYLE_RESET()         
#define P_LOCATION()             
  #define PF_IF_LOC()
#define PF(...)		    
#define P(msg)
#define P_INFO(msg)   
#define P_LINE()

#define P_SIGNED(v) 	
#define P_INT(v) 	    
#define P_S32(v) 	    
#define P_S16(v) 	    
#define P_S8(v) 	    
#define P_UNSIGNED(v) 
#define P_U32(v)      
#define P_U16(v)      
#define P_U8(v)       
#define P_F32(v) 	    
#define P_BOOL(v) 	  
#define P_CHAR(v) 	  
#define P_STR(v) 	    
#define P_TXT(v)      
#define P_ERR(...)	  
#define ASSERT(c)     
#define ERR(...)      
#define ERR_CHECK(c, ...)   
#define P_ERR_CHECK(c, ...) 

#define F32_NAN(v)  
#define P_NAN(v)    

#define GLOBAL_TEST_P_MACROS()
#define P_C_VERSION()                     

// -- func wrapper --

#define SPRINTF(max, ...)  sprintf(__VA_ARGS__) 
#define MALLOC(a, s)       (a) = malloc(s) 
#define CALLOC(n, s)       calloc(n, s)
#define REALLOC(p, s)      realloc(p, s)
#define FREE(n)            free(n)
// @UNSURE: stb_ds
#define ARRFREE(a)         arrfree((a)); (a) = NULL  
#define HMFREE(a)          hmfree((a));  (a) = NULL
#define SHFREE(a)          shfree((a));  (a) = NULL


#define DBG(func, ...)      (func)(__VA_ARGS__) 
#define DBG_F_L              

#endif // GLOBAL_DEBUG


#endif
