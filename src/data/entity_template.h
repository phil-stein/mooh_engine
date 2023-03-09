#ifndef ENTITY_DEF_H
#define ENTITY_DEF_H

#include "../global/global.h"
#include "../core/types/types.h"
#include "../math/math_inc.h"


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
}entity_template_type;


// template for entites
typedef struct entity_template_t
{
  char* name;

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
  vec3 aabb_offset; // offset from objects position

}entity_template_t;
#define ENTITY_TEMPLATE_T_SET_DEFAULTS()  \
  .name = "default",                      \
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
  .aabb_offset = { 0, 0, 0 },             \
  .is_trigger  = false                   

// removed () bc. vs19 is fckn stupid
#define ENTITY_TEMPLATE_T_EMPTY()         \
{                                         \
  .name = "empty",                        \
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
  .aabb_offset = { 0, 0, 0 },             \
  .is_trigger  = false                    \
}

const entity_template_t* entity_template_get(int idx);

const entity_template_t* entity_template_get_all(int* len);

#endif
