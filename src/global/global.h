#ifndef GLOBAL_H
#define GLOBAL_H

// libs needed basically everywhere
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>

// ifdef activates P... macros, ASSERT, ERR..., etc.
#define DEBUG


// null
#ifndef NULL
	#define NULL (void*)0
#endif

// bool
#ifndef bool
	#define bool int
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
typedef unsigned long long	u64;

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

#define P_INT(v) 	  PF("%s: %d\n", #v, v)
#define P_U32(v) 	  PF("%s: %u\n", #v, v)
#define P_F32(v) 	  PF("%s: %f\n", #v, v)
#define P_STR(v) 	  PF("%s: \"%s\"\n", #v, v)
#define P_CHAR(v) 	PF("%s: %c\n", #v, (char)(v))
#define P_BOOL(v) 	PF("%s: %s\n", #v, STR_BOOL(v))

#define P(msg)		  PF("%s\n", msg)
#define P_TXT(txt)  PF("%s:\n%s\n", #txt, txt)

// #define P_ERR(msg)	PF("[ERROR] %s\n -> file: %s, line: %d\n", msg, __FILE__, __LINE__)
#define P_ERR(...)	PF("[ERROR] "); PF(__VA_ARGS__); PF(" -> file: %s, line: %d\n", __FILE__, __LINE__)
#define ASSERT(c) if(!(c)) { PF("[ASSERT] '%s'\n -> file: %s, line: %d\n", #c, __FILE__, __LINE__); abort(); }
// #define ERR(msg)  P_ERR(msg); abort();
#define ERR(...)  P_ERR(__VA_ARGS__); abort();
// #define ERR_CHECK(c, msg) if(!(c)) { ERR(msg); }
#define ERR_CHECK(c, ...) if(!(c)) { ERR(__VA_ARGS__); }

#define P_INFO(msg) PF("[%s, %d] %s\n", __FILE__, __LINE__, msg)

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
