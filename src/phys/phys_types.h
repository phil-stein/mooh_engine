#ifndef PHYS_TYPES_H
#define PHYS_TYPES_H

#include "global/global.h"
#include "math/math_inc.h"

typedef struct collision_info_t
{
  bool collision;
  vec3 direction; // normal of collision
  f32  depth;
  int  obj_idx;   // idx into phys_objs array in phys_world.c, of other phys_obj_t
  bool trigger;

}collision_info_t;

typedef struct box_collider_t
{
  vec3 aabb[2];

}box_collider_t;
typedef struct sphere_collider_t
{
  f32 radius;

}sphere_collider_t;

typedef enum collider_type_t 
{ 
  PHYS_COLLIDER_SPHERE, 
  PHYS_COLLIDER_BOX 

} collider_type_t;
typedef struct collider_t
{
  collider_type_t type;
  vec3 offset;
  bool is_trigger;  
  union
  {
    sphere_collider_t sphere;
    box_collider_t    box;
  };

  collision_info_t* infos;
  int infos_len;

}collider_t;

typedef struct rigidbody_t
{
  vec3 velocity;
  vec3 force;
  f32  mass;
  f32  restitution;
  f32  static_friction;
  f32  dynamic_friction;
}rigidbody_t;

typedef enum phys_obj_flags_t 
{ 
  PHYS_HAS_RIGIDBODY = FLAG(0), 
  PHYS_HAS_BOX       = FLAG(1), 
  PHYS_HAS_SPHERE    = FLAG(2),

} phys_obj_flags_t;
#define PHYS_OBJ_HAS_RIGIDBODY(obj) (HAS_FLAG((obj)->flags, PHYS_HAS_RIGIDBODY))
#define PHYS_OBJ_HAS_COLLIDER(obj) \
  (HAS_FLAG((obj)->flags, PHYS_HAS_BOX) || HAS_FLAG((obj)->flags, PHYS_HAS_SPHERE))
typedef struct phys_obj_t
{
  u32  entity_idx;
  vec3 pos;
  vec3 scl;       

  phys_obj_flags_t flags;
  rigidbody_t rb;
  collider_t  collider;
}phys_obj_t;

#define PHYS_OBJ_INIT()       \
{                             \
  .entity_idx = -1,           \
  .pos        = { 0, 0, 0 },  \
  .flags      = 0,            \
}

#endif
