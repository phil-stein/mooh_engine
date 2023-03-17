#ifndef MATERIAL_TEMPLATE_H 
#define MATERIAL_TEMPLATE_H 

#include "global/global.h"
#include "data/shader_template.h"
#include "math/math_inc.h"

// @DOC: act as index for material_template_get()
//       ! if out of order materials will be loaded as the wrong material
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
  MATERIAL_TEMPLATE_PALADIN_BODY,
  MATERIAL_TEMPLATE_PALADIN_ARMOUR,
  MATERIAL_TEMPLATE_PALADIN_WEAPON,

}material_template_type;

// @DOC: template for material
//       specifies all data needed to make material
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
// @DOC: default values given to all templates in material_template.c 
//       and then individually overwritten if needed 
#define MATERIAL_TEMPLATE_DEFAULT_INIT()        \
    .albedo = NULL,                             \
    .normal = NULL,                             \
    .roughn = NULL,                             \
    .metall = NULL,                             \
    .tint   = { 1, 1, 1 },                      \
    .metall_f = 0.0f,                           \
    .roughn_f = 0.5f,                           \
    .tile_scl = 1.0f,                           \
    .tile_by_scl = false,                       \
    .tile = { 1, 1 },                           \
    .shader_template = SHADER_TEMPLATE_NONE

// @DOC: get pointer to material template by its index, see material_template_type
//       idx: index for template, use material_template_type
const material_template_t* material_template_get(int idx);

// @DOC: get all templates in arr
//       len: gets set to arr's length
const material_template_t* material_template_get_all(int* len);

#endif
