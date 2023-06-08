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

// typedef void (mui_quad_callback) ...
// typedef void (mui_quad_textured_f) ...
// typedef void (mui_quad_textured_tint_callback) ...

// void mui_init(mui_quad_callback quad_f, mui_quad_textured_f quad_tex_f, 
void mui_init();
void mui_update();

void mui_quad(ivec2 pos, vec2 size, rgb color);
void mui_quad_textured(ivec2 pos, vec2 size, texture_t* tex);
void mui_quad_textured_tint(ivec2 pos, vec2 size, texture_t* tex, rgb tint);

void mui_text(vec2 pos, char* text);
// void mui_text(ivec2 pos, ... txt, rgb color);


void mui_space();
void mui_group();

#endif  // MUI_MUI_H
