#ifndef ENTITY_DEF_H
#define ENTITY_DEF_H

#include "global/global.h"
#include "core/types/types.h"
#include "math/math_inc.h"


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
}entity_template_type;

typedef enum entity_templyte_phys_flags
{
  ENTITY_HAS_RIGIDBODY = FLAG(0),
  ENTITY_HAS_SPHERE    = FLAG(1),    // @TODO:   
  ENTITY_HAS_BOX       = FLAG(2),       
  ENTITY_HAS_PLANE     = FLAG(3),    // @TODO:   
  ENTITY_HAS_CONVEX    = FLAG(4),    // @TODO:     
  ENTITY_HAS_CAPSULE   = FLAG(5),    // @TODO:   

}entity_templyte_phys_flags;


// template for entites
typedef struct template_t
{
  char* name;

  char* mesh;
  int   mat;    // idx for material_template.c

  init_callback*   init;
  update_callback* update;

  // @TODO: physics, etc.
  u32 phys_flags;
  f32 mass;
  union
  {
    f32  radius;
    vec3 aabb_size; // total aabb size from min to max
  };
  

}template_t;


const template_t* template_get(int idx);

const template_t* template_get_all(int* len);

#endif
