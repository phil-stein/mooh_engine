#ifndef TERRAIN_H
#define TERRAIN_H

#include "../global/global.h"
#include "../math/math_inc.h"
#include "../core/types/mesh.h"
#include "../data/shader_template.h"
#include "../data/material_template.h"


typedef struct terrain_chunk_t
{
  bool loaded;
  bool visible;

  vec3 pos, scl;
  mat4 model;
  bool is_moved;  // 'dirtyflag' for the model matrix

  // opengl stuff
  u32 vao, vbo, ebo;
  u32 strips_num;
  u32 verts_per_strip;

  // for terrain_edit.c
#ifdef EDITOR
  f32* verts;
  u32  verts_len;
#endif

}terrain_chunk_t;

#define TERRAIN_FLOATS_PER_VERT 12

#ifdef EDITOR             
  #define CHUNK_INIT()      \
  {                         \
    .loaded = false,        \
    .visible = true,        \
    .pos = { 0, 0, 0 },     \
    .scl = { 1, 1, 1 },     \
    .is_moved = true,       \
    .strips_num = 0,        \
    .verts_per_strip = 0,   \
                            \
    .verts = NULL,          \
    .verts_len = 0          \
  }
#else
  #define CHUNK_INIT()      \
  {                         \
    .loaded = false,        \
    .visible = true,        \
    .pos = { 0, 0, 0 },     \
    .scl = { 1, 1, 1 },     \
    .is_moved = true,       \
    .strips_num = 0,        \
    .verts_per_strip = 0    \
  }
#endif                    
//  .rot = { 0, 0, 0 },    

typedef struct terrain_layout_t
{
  ivec2 pos;
  vec2* vert_info;  // [0]: height, [1]: material 
  // f32*  material;

}terrain_layout_t;
#define TERRAIN_LAYOUT_VERT_INFO_LEN(_core_data) ((_core_data)->terrain_x_len * (_core_data)->terrain_z_len)

void terrain_init();
void terrain_create(f32 uv_tile);
void terrain_update();

terrain_chunk_t terrain_generate_chunk(vec2* vert_info, u32 x_len, u32 z_len, f32 uv_tile);
void terrain_calculate_normals_tangents(f32* verts); 

void terrain_remove_chunk(u32 idx);

#ifdef EDITOR
void terrain_add_chunk(ivec2 pos);

void terrain_update_chunk_verts(int idx);
#endif

#endif
