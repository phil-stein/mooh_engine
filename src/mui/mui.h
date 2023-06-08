#ifndef MUI_MUI_H
#define MUI_MUI_H

#include "global/global.h"
#include "math/math_inc.h"
#include "core/types/texture.h"

typedef struct
{
  rgb color_text;
  rgb color_bg;   // idk some like dis

}mui_data;


typedef enum
{
  TEXT_UP     = FLAG(0),
  TEXT_MIDDLE = FLAG(1),
  TEXT_DOWN   = FLAG(2),

  TEXT_LEFT   = FLAG(3),
  TEXT_CENTER = FLAG(4),
  TEXT_RIGHT  = FLAG(5),
  
}text_orientation;

INLINE void P_TEXT_ORIENTATION(text_orientation o)     
{                               
  // PF("%s:", #o);                  
  PF("text_orientation: ");
  if (HAS_FLAG(o, TEXT_UP))     
  { PF("TEXT_UP | "); }
  if (HAS_FLAG(o, TEXT_MIDDLE)) 
  { PF("TEXT_MIDDLE | "); } 
  if (HAS_FLAG(o, TEXT_DOWN))   
  { PF("TEXT_DOWN | "); }   

  if (HAS_FLAG(o, TEXT_LEFT))   
  { PF("TEXT_LEFT | "); }   
  if (HAS_FLAG(o, TEXT_CENTER)) 
  { PF("TEXT_CENTER | "); } 
  if (HAS_FLAG(o, TEXT_RIGHT))  
  { PF("TEXT_RIGHT | "); }  
  
  PF("\n");
} 

// typedef void (mui_quad_callback) ...
// typedef void (mui_quad_textured_f) ...
// typedef void (mui_quad_textured_tint_callback) ...

// void mui_init(mui_quad_callback quad_f, mui_quad_textured_f quad_tex_f, 
void mui_init();
void mui_update();

void mui_quad(ivec2 pos, vec2 size, rgb color);
void mui_quad_textured(ivec2 pos, vec2 size, texture_t* tex);
void mui_quad_textured_tint(ivec2 pos, vec2 size, texture_t* tex, rgb tint);

void mui_text_ name hier lol (vec2 pos, char* text, text_orientation orientation);
#define mui_text(pos, text) mui_text((pos), (text), TEXT_UP | TEXT_LEFT)
// void mui_text(ivec2 pos, ... txt, rgb color);


void mui_space();
void mui_group();

#endif  // MUI_MUI_H
