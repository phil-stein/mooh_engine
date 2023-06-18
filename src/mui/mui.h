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
  // -- general --

  MUI_UP     = FLAG(0),
  MUI_MIDDLE = FLAG(1),
  MUI_DOWN   = FLAG(2),

  MUI_LEFT   = FLAG(3),
  MUI_CENTER = FLAG(4),
  MUI_RIGHT  = FLAG(5),
  
  // -- group --

  MUI_ROW    = FLAG(6),
  MUI_COLUMN = FLAG(7),
  MUI_GRID   = FLAG(8),

}mui_orientation_type;

INLINE void P_MUI_ORIENTATION(mui_orientation_type o)     
{                               
  // PF("%s:", #o);                  
  PF("mui_orientation_type: ");
  if (HAS_FLAG(o, MUI_UP))     
  { PF("MUI_UP | "); }
  if (HAS_FLAG(o, MUI_MIDDLE)) 
  { PF("MUI_MIDDLE | "); } 
  if (HAS_FLAG(o, MUI_DOWN))   
  { PF("MUI_DOWN | "); }   

  if (HAS_FLAG(o, MUI_LEFT))   
  { PF("TEXT_LEFT | "); }   
  if (HAS_FLAG(o, MUI_CENTER)) 
  { PF("MUI_CENTER | "); } 
  if (HAS_FLAG(o, MUI_RIGHT))  
  { PF("MUI_RIGHT | "); }  
  
  PF("\n");
} 

typedef enum 
{ 
  MUI_OBJ_TEXT = 1, 
  MUI_OBJ_QUAD = 2, 
  MUI_OBJ_IMG  = 3

}mui_obj_type;

#define MUI_OBJ_TEXT_MAX 32
typedef struct
{
  mui_obj_type type;

  vec2 pos;
  int text[MUI_OBJ_TEXT_MAX];
  int text_len;
  mui_orientation_type orientation;
  
  // vec2 pos;
  vec2 scl;
  rgbf color;
  texture_t* tex;

}mui_obj_t;
// @NOTE: text doesnt work like this
#define MUI_OBJ_T_INIT_TEXT(pos, text, orientation) \
{                                                   \
  .type = MUI_OBJ_TEXT,                             \
  .pos  = { (pos)[0], (pos)[1], (pos)[2] },         \
  .text = (text),                                   \
  .orientation = (orientation),                     \
}
// #define MUI_OBJ_T_INIT_QUAD(pos, scl, col)          \x
// {                                                   \x
//   .type  = MUI_OBJ_QUAD,                            \x
//   .pos   = { (pos)[0], (pos)[1] },                  \x
//   .scl   = { (scl)[0], (scl)[1] },                  \x
//   .color = { (col)[0], (col)[1], (col)[2] },        \x
// }
#define MUI_OBJ_T_INIT_QUAD(px, py, sx, sy, cr, cg, cb) \
{                                                       \
  .type  = MUI_OBJ_QUAD,                                \
  .pos   = { (px), (py) },                              \
  .scl   = { (sx), (sy) },                              \
  .color = { (cr), (cg), (cb) },                        \
}
#define MUI_OBJ_T_INIT_QUAD_GROUP(r, g, b)  (mui_obj_t)MUI_OBJ_T_INIT_QUAD(0, 0,  1, 1,   (r), (g), (b)) 

typedef struct
{
  vec2 pos;
  vec2 scl;
  f32  margin;

  rgbf  bg_color;
  bool  bg_visible;
  
  mui_obj_t* objs;
  int        objs_len;

  mui_orientation_type orientation;

  int max_wrap; // wrap after x mui_obj_t, -1 for no wrap

}mui_group_t;
#define MUI_GROUP_T_ADD(_group, _obj)  { arrput((_group)->objs, (_obj)); (_group)->objs_len++; }

// .pos   = { (pos)[0], (pos)[1] },        \x
// #define MUI_GROUP_T_INIT(pos_x, pos_y, scl_x, scl_y, margin, orientation) \x
// {                                                                         \x
//   .pos      = { (pos_x), (pos_y) },                                       \x
//   .scl      = { (scl_y), (scl_y) },                                       \x
//   .margin   = (margin),                                                   \x
//   .objs     = NULL,                                                       \x
//   .objs_len = 0,                                                          \x
//   .orientation = (orientation),                                           \x
// }
// #define MUI_GROUP_T_INIT(g, pos, scl, margin, orientation)  \x
// {                                                           \x
//   (g)->pos[0]      = (pos)[0];                              \x
//   (g)->pos[1]      = (pos)[1];                              \x
//   (g)->scl[0]      = (scl)[0];                              \x
//   (g)->scl[1]      = (scl)[1];                              \x
//   (g)->margin      = (margin),                              \x
//   (g)->objs        = NULL,                                  \x
//   (g)->objs_len    = 0,                                     \x
//   (g)->orientation = (orientation),                         \x
// }
INLINE void MUI_GROUP_T_INIT(mui_group_t* g, vec2 pos, vec2 scl, f32 margin, mui_orientation_type orientation)  
{                                                           
  (g)->pos[0]      = (pos)[0];                              
  (g)->pos[1]      = (pos)[1];                              
  (g)->scl[0]      = (scl)[0];                              
  (g)->scl[1]      = (scl)[1];                              
  (g)->margin      = (margin);                              
  (g)->objs        = NULL;                                  
  (g)->objs_len    = 0;  
  (g)->orientation = (orientation);                    
  (g)->max_wrap    = -1;  // no wrap  
}


void mui_init();
void mui_update();

// @DOC: draw text 
void mui_text(vec2 pos, char* text, mui_orientation_type orientation);
#define mui_text_l(pos, text) mui_text((pos), (text), TEXT_UP | TEXT_LEFT)
#define mui_text_r(pos, text) mui_text((pos), (text), TEXT_UP | TEXT_RIGHT)
// void mui_text(ivec2 pos, ... txt, rgb color);

// @DOC: register image or colored quad to be drawn in ui
#define mui_img(pos, scl, tex)            mui_img_tint((pos), (scl), (tex), VEC3(1))
#define mui_img_tint(pos, scl, tex, tint) mui_img_complx((pos), (scl), (tex), (tint), false)
int mui_img_complx(vec2 pos, vec2 scl, texture_t* tex, rgbf tint, bool scale_by_ratio);
int mui_quad(vec2 pos, vec2 scl, rgbf color);

// void mui_space();
void mui_group(mui_group_t* g);

#endif  // MUI_MUI_H
