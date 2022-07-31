#ifndef TYPES_H
#define TYPES_H

#include "global/global.h"
#include "core/types/texture.h"
#include "core/types/material.h" 
#include "core/types/mesh.h" 
#include "core/types/shader.h" 
#include "core/types/framebuffer.h"
#include "math/math_inc.h"


typedef struct cubemap_t
{
  u32 environment;
  u32 irradiance;
  u32 prefilter;
  // u32 brdf_lut;
}cubemap_t;

struct entity_t;
typedef void (init_callback)(struct entity_t* this);
typedef void (update_callback)(struct entity_t* this, float dt);

typedef enum entity_phys_flags
{
  ENTITY_HAS_RIGIDBODY = FLAG(0),
  ENTITY_HAS_SPHERE    = FLAG(1),    // @TODO:   
  ENTITY_HAS_BOX       = FLAG(2),       
  ENTITY_HAS_PLANE     = FLAG(3),    // @TODO:   
  ENTITY_HAS_CONVEX    = FLAG(4),    // @TODO:     
  ENTITY_HAS_CAPSULE   = FLAG(5),    // @TODO:   

}entity_phys_flags;

typedef struct entity_t
{
  u32 id;        // id for state_entity_get(id)
  int table_idx; // idx for entity_table_get(idx)
  bool is_dead;  // insteadf of deleting the entity from array, its marked dead and overwritten with the next added entity

  // local space
  vec3 pos;   // position
  vec3 rot;   // rotation
  vec3 scl;   // scale
  vec3 delta_pos;    // difference between last and current frame, used for physics sync 
  // vec3 delta_rot; // difference between last and current frame, used for physics sync, not needed as phys don't know what rotation is 
  vec3 delta_scl;    // difference between last and current frame, used for physics sync
  vec3 delta_force;  // difference between last and current frame, used for physics sync

  mat4 model;     // global space
  bool is_moved;  // specifies whether the entity has been moved since last model matrix update, used in 'state_entity_update_global_model()'

  int mat;  // index for assetm
  int mesh; // index for assetm

  // -- physics --
  entity_phys_flags phys_flags; // 0 if no flags, use HAS_FLAG() to check if flags are present
  bool is_grounded;             // only valid for ents with phys collider, otherwise always false

  // -> null or gets called at apropriate time
  init_callback*   init_f;
  update_callback* update_f;

  // int so they can be -1
  int  parent;
  int* children;
  int  children_len;

}entity_t;

// these set the 'is_moved' flag, these should always be used as otherwise the 'model' wont get updated, in 'state_entity_update_global_model()'
#define ENTITY_SET_POS(e, vec)      { vec3_sub((vec), (e)->pos, (e)->delta_pos); vec3_copy((vec), (e)->pos); (e)->is_moved = true; }  
#define ENTITY_SET_POS_X(e, x)      { (e)->delta_pos[0] += (x) - (e)->pos[0]; (e)->pos[0] = (x);    (e)->is_moved = true; }
#define ENTITY_SET_POS_Y(e, y)      { (e)->delta_pos[1] += (y) - (e)->pos[1]; (e)->pos[1] = (y);    (e)->is_moved = true; }
#define ENTITY_SET_POS_Z(e, z)      { (e)->delta_pos[2] += (z) - (e)->pos[2]; (e)->pos[2] = (z);    (e)->is_moved = true; }
#define ENTITY_MOVE(e, vec)         { vec3_add((e)->delta_pos, (vec), (e)->delta_pos); vec3_add((e)->pos,    (vec), (e)->pos); (e)->is_moved = true; }
#define ENTITY_MOVE_X(e, x)         { (e)->delta_pos[0] += (x);   (e)->pos[0] += (x);   (e)->is_moved = true; }
#define ENTITY_MOVE_Y(e, y)         { (e)->delta_pos[1] += (y);   (e)->pos[1] += (y);   (e)->is_moved = true; }
#define ENTITY_MOVE_Z(e, z)         { (e)->delta_pos[2] += (z);   (e)->pos[2] += (z);   (e)->is_moved = true; }
#define ENTITY_MOVE_AXIS(e, a, v)   { (e)->delta_pos[(a)] += (v); (e)->pos[(a)] += (v); (e)->is_moved = true; } // move on axis ' a '  from 0 -> 1

#define ENTITY_SET_ROT(e, vec)      { vec3_copy((vec),      (e)->rot); (e)->is_moved = true; }
#define ENTITY_SET_ROT_X(e, x)      { (e)->rot[0] = (x);    (e)->is_moved = true; }
#define ENTITY_SET_ROT_Y(e, y)      { (e)->rot[1] = (y);    (e)->is_moved = true; }
#define ENTITY_SET_ROT_Z(e, z)      { (e)->rot[2] = (z);    (e)->is_moved = true; }
#define ENTITY_ROTATE(e, vec)       { vec3_add((e)->rot,    (vec), (e)->rot); (e)->is_moved = true; }
#define ENTITY_ROTATE_X(e, x)       { (e)->rot[0] += (x);   (e)->is_moved = true; }
#define ENTITY_ROTATE_Y(e, y)       { (e)->rot[1] += (y);   (e)->is_moved = true; }
#define ENTITY_ROTATE_Z(e, z)       { (e)->rot[2] += (z);   (e)->is_moved = true; }
#define ENTITY_ROTATE_AXIS(e, a, v) { (e)->rot[(a)] += (v); (e)->is_moved = true; } // rotate on axis ' a '  from 0 -> 1

#define ENTITY_SET_SCL(e, vec)      { vec3 dif; vec3_sub((e)->scl, vec, dif), vec3_add(dif, (e)->delta_scl, (e)->delta_scl); vec3_copy((vec), (e)->scl); (e)->is_moved = true; }
#define ENTITY_SET_SCL_X(e, x)      { (e)->delta_scl[0] += (x) - (e)->scl[0]; (e)->scl[0] = (x);    (e)->is_moved = true; }
#define ENTITY_SET_SCL_Y(e, y)      { (e)->delta_scl[1] += (y) - (e)->scl[1]; (e)->scl[1] = (y);    (e)->is_moved = true; }
#define ENTITY_SET_SCL_Z(e, z)      { (e)->delta_scl[2] += (z) - (e)->scl[2]; (e)->scl[2] = (z);    (e)->is_moved = true; }
#define ENTITY_SCALE(e, vec)        { vec3_add((e)->delta_scl, (vec), (e)->delta_scl); vec3_add((e)->scl,    (vec), (e)->scl); (e)->is_moved = true; }
#define ENTITY_SCALE_X(e, x)        { (e)->delta_scl[0] += (x);   (e)->scl[0] += (x);   (e)->is_moved = true; }
#define ENTITY_SCALE_Y(e, y)        { (e)->delta_scl[1] += (y);   (e)->scl[1] += (y);   (e)->is_moved = true; }
#define ENTITY_SCALE_Z(e, z)        { (e)->delta_scl[2] += (z);   (e)->scl[2] += (z);   (e)->is_moved = true; }
#define ENTITY_SCALE_AXIS(e, a, v)  { (e)->delta_scl[(a)] += (v); (e)->scl[(a)] += (v); (e)->is_moved = true; } // scale on axis ' a '  from 0 -> 1

#define ENTITY_SET_FORCE(e, vec)    { vec3_copy((vec), (e)->delta_force); }
#define ENTITY_SET_FORCE_X(e, x)    { (e)->delta_force[0] = (x);   }
#define ENTITY_SET_FORCE_Y(e, y)    { (e)->delta_force[1] = (y);   }
#define ENTITY_SET_FORCE_Z(e, z)    { (e)->delta_force[2] = (z);   }
#define ENTITY_FORCE(e, vec)        { vec3_add((e)->delta_force, (vec), (e)->delta_force); }
#define ENTITY_FORCE_X(e, x)        { (e)->delta_force[0] += (x);   }
#define ENTITY_FORCE_Y(e, y)        { (e)->delta_force[1] += (y);   }
#define ENTITY_FORCE_Z(e, z)        { (e)->delta_force[2] += (z);   }
#define ENTITY_FORCE_AXIS(e, a, v)  { (e)->delta_force[(a)] += (v); } // scale on axis ' a '  from 0 -> 1

// @TODO:
typedef struct structure_t
{
  entity_t* entities;
  int len;
}structure_t;

typedef struct dir_light_t
{
  vec3  pos;
  vec3  dir;
  rgbf  color;
  // rgbf  ambient;
  // rgbf  diffuse;
  float intensity;

  bool cast_shadow;
  int  shadow_map_x;
  int  shadow_map_y;
  framebuffer_t fb_shadow;
  mat4 view;
  mat4 proj;

}dir_light_t;

typedef struct point_light_t
{
  vec3  pos;
  rgbf  color;
  float intensity;

}point_light_t;

typedef struct scene_t
{
  entity_t* world;
  int world_len;

  dir_light_t* dir_lights;
  int dir_lights_len;

  // point lights here

  // settings
  // e.g. cubemap
  // -> dir "cubemap_orange"
  // etc.

}scene_t;

// ??? how to save this
// -> shouldnt save material & shader each time
// -> use names (easier) or guids (faster) ?
//
// -- serialization --
// 
// int, float, vec, rgb, etc.
// -> direct
// mesh, shader, textures
// -> by name
//
// e.g. entity
// vec3 pos, rot, scl;
// char* mesh, shader, tex;
// vec3 tint;
//
// every asset is 'one' file
// -> .fbx, .png, .shader, etc.

#endif
