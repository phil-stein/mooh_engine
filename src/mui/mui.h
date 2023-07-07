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

  MUI_DYNAMIC = FLAG(6), 
  MUI_STATIC  = FLAG(7),

  MUI_ROW     = FLAG(8),
  MUI_COLUMN  = FLAG(9),
  MUI_GRID    = FLAG(10),

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
  bool active;        // determines if drawn or not

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
  .type   = MUI_OBJ_TEXT,                           \
  .active = true,                                   \
  .pos    = { (pos)[0], (pos)[1], (pos)[2] },       \
  .text   = (text),                                 \
  .orientation = (orientation),                     \
}

#define MUI_OBJ_T_INIT_QUAD(px, py, sx, sy, cr, cg, cb) \
{                                                       \
  .type  = MUI_OBJ_QUAD,                                \
  .active = true,                                       \
  .pos   = { (px), (py) },                              \
  .scl   = { (sx), (sy) },                              \
  .color = { (cr), (cg), (cb) },                        \
}
#define MUI_OBJ_T_INIT_QUAD_GROUP(r, g, b)  MUI_OBJ_T_INIT_QUAD(0, 0,  1, 1,   (r), (g), (b)) 
#define MUI_OBJ_T_QUAD_GROUP(r, g, b)       (mui_obj_t)MUI_OBJ_T_INIT_QUAD(0, 0,  1, 1,   (r), (g), (b)) 

#define MUI_OBJ_T_INIT_IMG(px, py, sx, sy, _tex, cr, cg, cb) \
{                                                            \
  .type  = MUI_OBJ_IMG,                                      \
  .active = true,                                            \
  .pos   = { (px), (py) },                                   \
  .scl   = { (sx), (sy) },                                   \
  .color = { (cr), (cg), (cb) },                             \
  .tex   = (_tex),                                           \
}
#define MUI_OBJ_T_IMG(_px, _py, _sx, _sy, _tex, r, g, b)  (mui_obj_t)MUI_OBJ_T_INIT_IMG((_px), (_py), (_sx), (_sy), (_tex),  (r), (g), (b))
#define MUI_OBJ_T_INIT_IMG_GROUP(_tex, r, g, b)           MUI_OBJ_T_INIT_IMG(0, 0,  1, 1,  (_tex),  (r), (g), (b)) 
#define MUI_OBJ_T_IMG_GROUP(_tex, r, g, b)                (mui_obj_t)MUI_OBJ_T_INIT_IMG(0, 0,  1, 1,  (_tex),  (r), (g), (b))

typedef struct
{
  vec2 pos;
  vec2 scl;
  f32  margin;
  bool scale_by_ratio; 

  rgbf  bg_color;
  bool  bg_visible;
  
  mui_obj_t* objs;
  int        objs_len;

  mui_orientation_type orientation;

  int max_wrap; // wrap after x mui_obj_t, -1 for no wrap

}mui_group_t;
#define MUI_GROUP_T_ADD(_group, _obj)           { arrput((_group)->objs, (_obj)); (_group)->objs_len++; }
#define MUI_GROUP_T_ADD_PTR(_group, _obj, _ptr)       \
      {                                               \
        arrput((_group)->objs, (_obj));               \
        (_ptr) = &(_group)->objs[(_group)->objs_len]; \
        (_group)->objs_len++;                         \
      }

INLINE void MUI_GROUP_T_INIT(mui_group_t* g, vec2 pos, vec2 scl, f32 margin, mui_orientation_type orientation, bool scale_by_ratio)  
{                                                           
  (g)->pos[0]         = (pos)[0];                              
  (g)->pos[1]         = (pos)[1];                              
  (g)->scl[0]         = (scl)[0];                              
  (g)->scl[1]         = (scl)[1];                              
  (g)->margin         = (margin);                              
  (g)->objs           = NULL;                                  
  (g)->objs_len       = 0;  
  (g)->orientation    = (orientation);                    
  (g)->max_wrap       = -1;  // no wrap
  (g)->scale_by_ratio = scale_by_ratio;
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
void mui_img_complx(vec2 pos, vec2 scl, texture_t* tex, rgbf tint, bool scale_by_ratio);
void mui_img_obj(mui_obj_t* obj, bool scale_by_ratio);
int mui_quad(vec2 pos, vec2 scl, rgbf color);

// void mui_space();
void mui_group(mui_group_t* g);

#endif  // MUI_MUI_H
