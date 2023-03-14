#ifndef GLOBAL_H
#define GLOBAL_H


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


// ifdef activates P... macros, ASSERT, ERR..., etc.
#define DEBUG


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

// #define INLINE static inline __attribute( (always_inline) )
#if defined(_MSC_VER)
#  define INLINE __forceinline
#else
#  define INLINE static inline __attribute((always_inline))
#endif

// ---- helper ----

#define FLAG(a) (1 << (a)) // aka. 2^a, 0, 2, 4, 8, ...

// a: 00010010
// b: 00000010
// &: 00000010 true bc. >1
#define HAS_FLAG(a, b)   ((a) & (b))

#define REMOVE_FLAG(a, b) ((a) &= ~(b))


#define STR_VAR(v)  (#v)
#define STR_BOOL(v) ((v) ? "true" : "false")

#ifdef DEBUG

// -- print --


#define PF(...)		  printf(__VA_ARGS__)
#define P(msg)		  PF("%s\n", msg)
#define P_INFO(msg) PF_COLOR(PF_YELLOW); PF("[INFO] "); PF_STYLE_RESET(); PF("%s\n", msg); PF_STYLE(PF_DIM, PF_WHITE); PF_STYLE(PF_ITALIC, PF_WHITE); PF(" -> file: %s, line: %d\n", __FILE__, __LINE__); PF_STYLE_RESET()

// draw --- line as long as the current console is wide, only works on windows
#define P_LINE()    { int w, h; io_util_get_console_size_win(&w, &h); for (int i = 0; i < w -1; ++i) { PF("-"); } PF("\n"); }

#define P_SIGNED(v) 	PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": %d\n", (v))
#define P_INT(v) 	    P_SIGNED(v)
#define P_S32(v) 	    P_SIGNED(v) 
#define P_S16(v) 	    P_SIGNED(v) 
#define P_S8(v) 	    P_SIGNED(v) 
#define P_UNSIGNED(v) PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": %u\n", (v))
#define P_U32(v)      P_UNSIGNED(v)
#define P_U16(v)      P_UNSIGNED(v)
#define P_U8(v)       P_UNSIGNED(v)

#define P_F32(v) 	    PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": %f\n", (v))
#define P_BOOL(v) 	  PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": %s\n", STR_BOOL(v))

#define P_CHAR(v) 	  PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": '%c'\n", (char)(v))
#define P_STR(v) 	    PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(": \"%s\"\n", (v)) 
#define P_TXT(v)      PF_COLOR(PF_CYAN); PF("%s", #v); PF_STYLE_RESET(); PF(":\n%s\n", (v))    

// #define P_ERR(msg)	PF("[ERROR] %s\n -> file: %s, line: %d\n", msg, __FILE__, __LINE__)
#define P_ERR(...)	PF_COLOR(PF_RED); PF("[ERROR] "); PF_MODE_RESET(); PF(__VA_ARGS__); PF_STYLE(PF_DIM, PF_WHITE); PF_STYLE(PF_ITALIC, PF_WHITE); PF(" -> file: %s, line: %d\n", __FILE__, __LINE__); PF_STYLE_RESET()
#define ASSERT(c)   if(!(c)) { PF_COLOR(PF_RED); PF("[ASSERT]" ); PF_MODE_RESET(); PF("'%s'\n", #c); PF_STYLE(PF_DIM, PF_WHITE); PF_STYLE(PF_ITALIC, PF_WHITE); PF(" -> file: %s, line: %d\n", __FILE__, __LINE__); PF_STYLE_RESET(); abort(); }
// #define ERR(msg)  P_ERR(msg); abort();
#define ERR(...)  P_ERR(__VA_ARGS__); abort();
// #define ERR_CHECK(c, msg) if(!(c)) { ERR(msg); }
#define ERR_CHECK(c, ...) if(!(c)) { ERR(__VA_ARGS__); }
#define P_ERR_CHECK(c, ...) if(!(c)) { P_ERR(__VA_ARGS__); }

// print is f32 is nan
#define F32_NAN(v)  (isnan(v) != 0)
#define P_NAN(v)    { if (F32_NAN(v)) { PF("%s is nan\n", #v); } }


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

#define PF_MODE(style, fg, bg)   PF("\033[%d;%d;%dm", style, fg, bg)
#define PF_STYLE(style, color)   PF("\033[%d;%dm", style, color)
#define PF_COLOR(color)          PF_STYLE(PF_NORMAL, color)
#define PF_MODE_RESET()          PF_MODE(PF_NORMAL, PF_WHITE, PF_BG_BLACK)
#define PF_STYLE_RESET()         PF_STYLE(PF_NORMAL, PF_WHITE)


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

#define SPRINTF(max, ...)  ASSERT(sprintf(__VA_ARGS__) < (max)) 

#define MALLOC(a, s)       {(a) = malloc(s); ASSERT((a) != NULL); }
#define CALLOC(n, s)       ASSERT(calloc(n, s) != NULL)
#define REALLOC(p, s)      ASSERT(realloc(p, s) != NULL)
#define FREE(n)            ASSERT(n != NULL); free(n)

#elif

#define PF(...)		  
#define P_INT(v) 	  
#define P_F32(v) 	  
#define P_STR(v) 	  
#define P_CHAR(v) 	
#define P_BOOL(v) 	
#define P(msg)		  
#define P_IL(msg)	  
#define P_TXT(txt)  
#define P_ERR(msg)	
#define P_ERRF(...)	
#define ASSERT(c) 
#define ERR(msg)  
#define ERRF(...)  
#define ERR_CHECK(c, msg) 
#define ERR_CHECKF(c, ...) 

#endif


#endif
