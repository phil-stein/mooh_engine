#ifndef ENTITY_DEF_H
#define ENTITY_DEF_H

#include "../global/global.h"
#include "../core/types/types.h"
#include "../math/math_inc.h"


// @DOC: act as index for entity_template_get()
//       ! if out of order entities will be loaded as the wrong entity
typedef enum entity_template_type 
{
  ENTITY_TEMPLATE_QUAD,
  ENTITY_TEMPLATE_SPHERE01,
  ENTITY_TEMPLATE_SPHERE02,
  ENTITY_TEMPLATE_DEMON01,
  ENTITY_TEMPLATE_SHOTGUN,
  ENTITY_TEMPLATE_DEMON02,
  ENTITY_TEMPLATE_STONE01,
  ENTITY_TEMPLATE_TREE01,
  ENTITY_TEMPLATE_HUT_TEST,
  ENTITY_TEMPLATE_CUBE_STATIC,
  ENTITY_TEMPLATE_CUBE_DYNAMIC,
  ENTITY_TEMPLATE_PLAYER,
  ENTITY_TEMPLATE_CUBE_STATIC_TRIGGER,
  ENTITY_TEMPLATE_PALADIN_BODY,
  ENTITY_TEMPLATE_PALADIN_ARMOUR,
  ENTITY_TEMPLATE_PALADIN_WEAPON,
  ENTITY_TEMPLATE_SPHERE_DYN,

}entity_template_type;


// @DOC: template for entity
//       specifies all data needed to make entity except transform
typedef struct entity_template_t
{
  char* name;
  s64   tags_flag;  // 0 is no tags

  char* mesh;   // name for assetm, "-" means no mesh
  int   mat;    // idx for material_template.c, -1 means no mesh

  init_callback*      init_f;
  update_callback*    update_f;
  collision_callback* collision_f;
  trigger_callback*   trigger_f;

  entity_phys_flag phys_flag;
  f32 mass;
  f32 friction;
  union
  {
    f32  radius;
    vec3 aabb_size;   // total aabb size from min to max
  };
  bool is_trigger;
  vec3 collider_offset; // offset from objects position

}entity_template_t;
// @DOC: default values given to all templates in entity_template.c 
//       and then individually overwritten if needed 
#define ENTITY_TEMPLATE_T_SET_DEFAULTS()  \
  .name = "default",                      \
  .tags_flag = 0,                         \
  .mesh = "cube",                         \
  .mat  = MATERIAL_TEMPLATE_DEFAULT,      \
  .init_f      = NULL,                    \
  .update_f    = NULL,                    \
  .collision_f = NULL,                    \
  .trigger_f   = NULL,                    \
  .phys_flag   = 0,                       \
  .mass        = 1.0f,                    \
  .friction    = 0.1f,                    \
  .aabb_size   = { 1, 1, 1 },             \
  .collider_offset = { 0, 0, 0 },         \
  .is_trigger  = false                   

// @DOC: values for an empty entity
#define ENTITY_TEMPLATE_T_EMPTY()         \
{                                         \
  .name = "empty",                        \
  .tags_flag = 0,                         \
  .mesh = "-",                            \
  .mat  = 0,                              \
  .init_f      = NULL,                    \
  .update_f    = NULL,                    \
  .collision_f = NULL,                    \
  .trigger_f   = NULL,                    \
  .phys_flag   = 0,                       \
  .mass        = 1.0f,                    \
  .friction    = 0.1f,                    \
  .aabb_size   = { 1, 1, 1 },             \
  .collider_offset = { 0, 0, 0 },         \
  .is_trigger  = false                    \
}

// @DOC: get pointer to entity template by its index, see entity_template_type
//       idx: index for template, use entity_template_type
const entity_template_t* entity_template_get(int idx);

// @DOC: get all templates in arr
//       len: gets set to arr's length
const entity_template_t* entity_template_get_all(int* len);

#endif
