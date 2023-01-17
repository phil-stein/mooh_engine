#ifndef MATERIAL_TEMPLATE_H 
#define MATERIAL_TEMPLATE_H 

#include "global/global.h"
#include "data/shader_template.h"
#include "math/math_inc.h"

typedef enum material_template_type
{
  MATERIAL_TEMPLATE_EMPTY,
  MATERIAL_TEMPLATE_DEFAULT,
  MATERIAL_TEMPLATE_METALL,
  MATERIAL_TEMPLATE_DEMON01,
  MATERIAL_TEMPLATE_DEMON02,
  MATERIAL_TEMPLATE_SHOTGUN,
  MATERIAL_TEMPLATE_GRASS,
  MATERIAL_TEMPLATE_STONE01,
  MATERIAL_TEMPLATE_TREE01,
  MATERIAL_TEMPLATE_PATH,
  MATERIAL_TEMPLATE_PLANKS,
  MATERIAL_TEMPLATE_BRICK,

}material_template_type;


typedef struct material_template_t
{
  const char* albedo;
  const char* normal;
  const char* roughn;
  const char* metall;
  const rgbf  tint;
  const float metall_f;
  const float roughn_f;

  const f32   tile_scl;    // getts mul with tile
  const bool  tile_by_scl; // scale uv by ent scl
  const vec2  tile;        // tile/scale uv coords
  
  shader_template_type shader_template;  // SHADER_TEMPLATE_NONE for default

}material_template_t;

// @NOTE: is this better, than having the values in the .c file ?
#define MATERIAL_TEMPLATE_DEFAULT_INIT()        \
    .albedo = NULL,                             \
    .normal = NULL,                             \
    .roughn = NULL,                             \
    .metall = NULL,                             \
    .tint   = (vec3){ 1, 1, 1 },                \
    .metall_f = 0.0f,                           \
    .roughn_f = 0.5f,                           \
    .tile_scl = 1.0f,                           \
    .tile_by_scl = false,                       \
    .tile = (vec2){ 1, 1 },                     \
    .shader_template = SHADER_TEMPLATE_NONE


const material_template_t* material_template_get(int idx);
const material_template_t* material_template_get_all(int* len);

#endif
