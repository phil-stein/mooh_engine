#include "core/terrain.h"
#include "core/io/assetm.h"
#include "core/core_data.h"
#include "core/camera.h"
#include "core/window.h"
#include "core/renderer/renderer.h"      // ID_BUFFER_TO_CHUNK_IDX()
#include "core/types/types.h"
#include "core/debug/debug_timer.h"
#include "core/debug/debug_draw.h"
#include "math/math_inc.h"

#include "GLAD/glad.h"
#include "stb/stb_ds.h"

// tutorial: https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

static core_data_t* core_data = NULL;

#ifdef EDITOR
vec2* vert_info_zero = NULL; // for terrain_add_chunk()
#endif

// void terrain_tesselation_init();
// void terrain_smooth_normals(u32* indices, f32* verts, int width, int height);
// void terain_generate_tangents(u32* indices, f32* verts);
// void terain_generate_tangents(f32* verts);

void terrain_init()
{
  core_data = core_data_get();
}

void terrain_create(f32 uv_tile)
{
  ERR_CHECK(core_data->terrain_cull_dist > core_data->terrain_draw_dist, "cull dist has to be greater as otherwise active chunks get removed.");

  
  arrput(core_data->terrain_materials, assetm_get_material_idx(MATERIAL_TEMPLATE_GRASS));
  core_data->terrain_materials_len++;
  arrput(core_data->terrain_materials, assetm_get_material_idx(MATERIAL_TEMPLATE_PATH));
  core_data->terrain_materials_len++;

  // vec3_copy(VEC3_XYZ(core_data->terrain_scl, core_data->terrain_y_scl, core_data->terrain_scl), core_data->terrain_chunk_scl);

  // core_data->terrain_chunks = malloc(sizeof(terrain_chunk_t) * core_data->terrain_layout_len);
  core_data->terrain_chunks_len = core_data->terrain_layout_len;
  terrain_chunk_t chunk_init = CHUNK_INIT();
  for (u32 i = 0; i < core_data->terrain_chunks_len; ++i)
  {
    // core_data->terrain_chunks[i] = chunk_init;
    arrput(core_data->terrain_chunks, chunk_init);
  }

}

void terrain_update()
{
  vec3 cam_pos;
  vec3_copy(core_data->cam.pos, cam_pos); // camera_get_pos(cam_pos);
  for (u32 i = 0; i < core_data->terrain_chunks_len; ++i)
  {
    terrain_chunk_t* chunk = &core_data->terrain_chunks[i];
    if (vec3_distance(cam_pos, chunk->pos) <= core_data->terrain_draw_dist * core_data->terrain_scl) // in range
    {
      if (chunk->loaded && !chunk->visible) { chunk->visible = true; }
      else if (!chunk->loaded) 
      { 
        terrain_layout_t* l = &core_data->terrain_layout[i];
        terrain_chunk_t chunk = terrain_generate_chunk(l->vert_info, 
            core_data->terrain_x_len, core_data->terrain_z_len, 25.0f);
        chunk.loaded = true;
        
        vec3 pos = { l->pos[0] * core_data->terrain_scl - l->pos[0], 0.0f, (l->pos[1] * core_data->terrain_scl) - l->pos[1] };
        vec3_copy(pos, chunk.pos);
        // vec3_copy(VEC3(0), chunk.rot);
        vec3_copy(VEC3(core_data->terrain_scl), chunk.scl);
        
        core_data->terrain_chunks[i] = chunk;
      }
    }
    else { chunk->visible = false; }  
   
    if (chunk->loaded && vec3_distance(cam_pos, chunk->pos) >= core_data->terrain_cull_dist * core_data->terrain_scl)
    {
      // PF(" -> remove chunk: %d\n", i);
      terrain_remove_chunk(i);
    }
  }
}

terrain_chunk_t terrain_generate_chunk(vec2* vert_info, u32 x_len, u32 z_len, f32 uv_tile)
{
  terrain_chunk_t chunk = CHUNK_INIT();

  f32* verts = NULL;
  u32* indices = NULL;

  u32 info_idx = 0;
  for (u32 z = 0; z < z_len; ++z)
  {
    for (u32 x = 0; x < x_len; ++x)
    {
      f32 y   = vert_info[info_idx][0]; // PF(" -> x: %u, z: %u, pos: %u\n", x, z, height_idx);
      info_idx++;

      // position
      arrput(verts, ((f32)x / (f32)x_len) - 0.5f);        
      arrput(verts, y);    
      arrput(verts, ((f32)z / (f32)z_len) - 0.5f);                        
      // normal
      arrput(verts, 0);
      arrput(verts, 1);
      arrput(verts, 0);
      // uv's
      arrput(verts, ((f32)x / (f32)x_len) * uv_tile);
      arrput(verts, ((f32)z / (f32)z_len) * uv_tile);
      // tangent
      arrput(verts, 0);
      arrput(verts, 0);
      arrput(verts, 0);
      // material
      // arrput(verts, mat);
      arrput(verts, vert_info[info_idx][1]);
    }
  }
  
  for(u32 z = 0; z < z_len-1; z++)       // for each row a.k.a. each strip
  {
    for(u32 x = 0; x < x_len; x++)        // for each column
    {
      for(int k = 0; k < 2; k++)          // for each side of the strip
      {
        arrput(indices, x + x_len * (z + k));
      }
    }
  }
  chunk.strips_num      = z_len -1;
  chunk.verts_per_strip = x_len *2;

  TIMER_FUNC_STATIC(terrain_calculate_normals_tangents(verts));

  // register VAO
  glGenVertexArrays(1, &chunk.vao);
  glBindVertexArray(chunk.vao);

  glGenBuffers(1, &chunk.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);
  glBufferData(GL_ARRAY_BUFFER,
      arrlen(verts) * sizeof(float),       // size of vertices buffer
      verts,                               // pointer to first element
      GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
      TERRAIN_FLOATS_PER_VERT * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
      TERRAIN_FLOATS_PER_VERT * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
  // uv's
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
      TERRAIN_FLOATS_PER_VERT * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// tangents
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 
      TERRAIN_FLOATS_PER_VERT * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// material
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 
      TERRAIN_FLOATS_PER_VERT * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

  glGenBuffers(1, &chunk.ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      arrlen(indices) * sizeof(u32),          // size of indices buffer
      indices,                                // pointer to first element
      GL_STATIC_DRAW);

  // needed for terrain_edit.c
#ifdef EDITOR
  chunk.verts       = verts;
  chunk.verts_len   = arrlen(verts);
#else
  ARRFREE(verts);
#endif
  ARRFREE(indices);

  return chunk;
}

void terrain_remove_chunk(u32 idx)
{
  ERR_CHECK(idx <= core_data->terrain_chunks_len, "idx too high: %d", idx);
  terrain_chunk_t* chunk = &core_data->terrain_chunks[idx];
  ERR_CHECK(chunk->loaded, "chunk not loaded: idx: %d", idx);
  
  glDeleteVertexArrays(1, &chunk->vao);
  glDeleteBuffers(1, &chunk->vbo);
  glDeleteBuffers(1, &chunk->ebo);

  // needeed for terrain_edit.c
#ifdef EDITOR
  ARRFREE(chunk->verts);
  chunk->verts_len = 0;
#endif

  chunk->loaded  = false;
  chunk->visible = false;
}

#ifdef EDITOR
void terrain_add_chunk(ivec2 pos)
{
  // check if already exists
  for (u32 i = 0; i < core_data->terrain_layout_len; ++i)
  {
    if (core_data->terrain_layout[i].pos[0] == pos[0] && 
          core_data->terrain_layout[i].pos[1] == pos[1] ) 
    { return; }
  }

  // make / use existing vert info array
  if (vert_info_zero == NULL)
  {
    MALLOC(vert_info_zero, sizeof(vec2) * TERRAIN_LAYOUT_VERT_INFO_LEN(core_data));
    for (int i = 0; i < TERRAIN_LAYOUT_VERT_INFO_LEN(core_data); ++i)
    { vert_info_zero[i][0] = 0.0f; vert_info_zero[i][1] = 0.0f; }
  }

  terrain_layout_t layout = { .pos = { pos[0], pos[1] }, .vert_info = vert_info_zero  };
  arrput(core_data->terrain_layout, layout);
  core_data->terrain_layout_len++;
  
  // core_data->terrain_chunks = realloc(core_data->terrain_chunks, sizeof(terrain_chunk_t) * core_data->terrain_layout_len);
  arrput(core_data->terrain_chunks, (terrain_chunk_t)CHUNK_INIT());
  core_data->terrain_chunks_len = core_data->terrain_layout_len;
  // core_data->terrain_chunks[core_data->terrain_chunks_len] = (terrain_chunk_t)CHUNK_INIT();

}
#endif

#ifdef EDITOR
void terrain_update_chunk_verts(int idx)
{
  ERR_CHECK(idx <= core_data->terrain_chunks_len, "idx too high: %d", idx);
  terrain_chunk_t* chunk = &core_data->terrain_chunks[idx];
  ERR_CHECK(chunk->loaded, "chunk not loaded: idx: %d", idx);
 
  // copy to layout
  for (u32 i = 0; i < TERRAIN_LAYOUT_VERT_INFO_LEN(core_data); ++i)
  {
    // height & material
    core_data->terrain_layout[idx].vert_info[i][0] = chunk->verts[(i * TERRAIN_FLOATS_PER_VERT) +1];
    core_data->terrain_layout[idx].vert_info[i][1] = chunk->verts[(i * TERRAIN_FLOATS_PER_VERT) +11];
  }

  terrain_calculate_normals_tangents(chunk->verts);
  
  glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	glBufferData(GL_ARRAY_BUFFER, chunk->verts_len * sizeof(f32), chunk->verts, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);  
}
#endif

void terrain_calculate_normals_tangents(f32* verts)  
{
  u32 x_len = core_data->terrain_x_len;
  u32 len    = x_len * core_data->terrain_z_len;
  for (u32 i = 0; i < len - x_len; ++i)
  {
    u32 idx0 = 0;
    u32 idx1 = 0;
    u32 idx2 = 0;
    // z edges
    if (i % x_len == x_len -1)
    {
      // @BUGG: wrong tangents
      idx0 = (i +0)     * TERRAIN_FLOATS_PER_VERT;
      idx1 = (i -1)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i -2)     * TERRAIN_FLOATS_PER_VERT;

      idx1 = (i +1)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i +2)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i +x_len) * TERRAIN_FLOATS_PER_VERT;
    }
    else
    {
      idx0 = (i +0)     * TERRAIN_FLOATS_PER_VERT;
      idx1 = (i +1)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i +x_len) * TERRAIN_FLOATS_PER_VERT;
    }
    vec3 pos0 = { verts[idx0 + 0], verts[idx0 +1], verts[idx0 +2] }; 
    vec3 pos1 = { verts[idx1 + 0], verts[idx1 +1], verts[idx1 +2] }; 
    vec3 pos2 = { verts[idx2 + 0], verts[idx2 +1], verts[idx2 +2] }; 

    // @NOTE: draw pos0
    // if (i == len -1)
    // if (i % x_len == x_len -1)
    // {
    //   vec3_mul_f(pos0, core_data->terrain_scl, pos0);
    //   debug_draw_sphere_register(pos0, 0.25f, RGB_F(1.0f, 1.0f, 1.0f));
    // }

    vec3 v01, v02;
    vec3_sub(pos1, pos0, v01);
    vec3_sub(pos2, pos0, v02);

    vec3 norm;
    vec3_cross(v02, v01, norm);
  
    // -- tangents --

    vec2 tex0 = { verts[idx0 + 6], verts[idx0 + 7] };
    vec2 tex1 = { verts[idx1 + 6], verts[idx1 + 7] };
    vec2 tex2 = { verts[idx2 + 6], verts[idx2 + 7] };

    vec2 uv0, uv1;
    vec2_sub(tex1, tex0, uv0);
    vec2_sub(tex2, tex0, uv1);


    vec3 edge0, edge1;
    vec3_sub(pos1, pos0, edge0);
    vec3_sub(pos2, pos0, edge1); 

    f32 r = 1.0f / (uv0[0] * uv1[1] - uv0[1] * uv1[0]);

    vec3 tan = VEC3_XYZ_INIT(
        ((edge0[0] * uv1[1]) - (edge1[0] * uv0[1])) * r,
        ((edge0[1] * uv1[1]) - (edge1[1] * uv0[1])) * r,
        ((edge0[2] * uv1[1]) - (edge1[2] * uv0[1])) * r
        );

    // -- overwritting values --
    verts[idx0 +  3] = norm[0];
    verts[idx0 +  4] = norm[1];
    verts[idx0 +  5] = norm[2];
    verts[idx0 +  8] = tan[0]; 
    verts[idx0 +  9] = tan[1]; 
    verts[idx0 + 10] = tan[2]; 

  }
  // // z edges
  // for (u32 i = x_len -1; i < len - x_len; i += x_len)
  // {
  //   u32 idx0 = 0;
  //   u32 idx1 = 0;
  //   u32 idx2 = 0;
  //   
  //   // @BUGG: wrong tangents
  //   idx0 = (i +0)     * TERRAIN_FLOATS_PER_VERT;
  //   idx1 = (i -1)     * TERRAIN_FLOATS_PER_VERT;
  //   idx2 = (i -2)     * TERRAIN_FLOATS_PER_VERT;

  //   idx1 = (i +1)     * TERRAIN_FLOATS_PER_VERT;
  //   idx2 = (i +2)     * TERRAIN_FLOATS_PER_VERT;
  //   idx2 = (i +x_len) * TERRAIN_FLOATS_PER_VERT;

  //   vec3 pos0 = { verts[idx0 + 0], verts[idx0 +1], verts[idx0 +2] }; 
  //   vec3 pos1 = { verts[idx1 + 0], verts[idx1 +1], verts[idx1 +2] }; 
  //   vec3 pos2 = { verts[idx2 + 0], verts[idx2 +1], verts[idx2 +2] }; 
  //    
  //   vec2 tex0 = { verts[idx0 + 6], verts[idx0 + 7] };
  //   vec2 tex1 = { verts[idx1 + 6], verts[idx1 + 7] };
  //   vec2 tex2 = { verts[idx2 + 6], verts[idx2 + 7] };

  //   vec2 uv0, uv1;
  //   vec2_sub(tex1, tex0, uv0);
  //   vec2_sub(tex2, tex0, uv1);


  //   vec3 edge0, edge1;
  //   vec3_sub(pos1, pos0, edge0);
  //   vec3_sub(pos2, pos0, edge1); 

  //   f32 r = 1.0f / (uv0[0] * uv1[1] - uv0[1] * uv1[0]);

  //   vec3 tan = VEC3_XYZ_INIT(
  //       ((edge0[0] * uv1[1]) - (edge1[0] * uv0[1])) * r,
  //       ((edge0[1] * uv1[1]) - (edge1[1] * uv0[1])) * r,
  //       ((edge0[2] * uv1[1]) - (edge1[2] * uv0[1])) * r
  //       );   
  //   // vec3_copy(VEC3_XYZ(0, 0, 1), tan);

  //   verts[idx0 +  8] = tan[0]; 
  //   verts[idx0 +  9] = tan[1]; 
  //   verts[idx0 + 10] = tan[2]; 
  // }
  // last row
  for (u32 i = len - x_len; i < len; ++i)
  {
    u32 idx0 = 0;
    u32 idx1 = 0; 
    u32 idx2 = 0; 
    // first vert
    if (i == len -x_len)
    {
      idx0 = (i -0)     * TERRAIN_FLOATS_PER_VERT;
      idx1 = (i +1)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i +2)     * TERRAIN_FLOATS_PER_VERT;
    }
    else
    {
      idx0 = (i -0)     * TERRAIN_FLOATS_PER_VERT;
      idx1 = (i -1)     * TERRAIN_FLOATS_PER_VERT;
      idx2 = (i -x_len) * TERRAIN_FLOATS_PER_VERT;
    }
    vec3 pos0 = { verts[idx0 + 0], verts[idx0 +1], verts[idx0 +2] }; 
    vec3 pos1 = { verts[idx1 + 0], verts[idx1 +1], verts[idx1 +2] }; 
    vec3 pos2 = { verts[idx2 + 0], verts[idx2 +1], verts[idx2 +2] }; 


    vec3 v01, v02;
    vec3_sub(pos1, pos0, v01);
    vec3_sub(pos2, pos0, v02);

    vec3 norm;
    vec3_cross(v02, v01, norm);

    // -- tangents --

    vec2 tex0 = { verts[idx0 + 6], verts[idx0 + 7] };
    vec2 tex1 = { verts[idx1 + 6], verts[idx1 + 7] };
    vec2 tex2 = { verts[idx2 + 6], verts[idx2 + 7] };

    vec2 uv0, uv1;
    vec2_sub(tex1, tex0, uv0);
    vec2_sub(tex2, tex0, uv1);


    vec3 edge0, edge1;
    vec3_sub(pos1, pos0, edge0);
    vec3_sub(pos2, pos0, edge1);

    f32 r = 1.0f / (uv0[0] * uv1[1] - uv0[1] * uv1[0]);

    vec3 tan = VEC3_XYZ_INIT(
        ((edge0[0] * uv1[1]) - (edge1[0] * uv0[1])) * r,
        ((edge0[1] * uv1[1]) - (edge1[1] * uv0[1])) * r,
        ((edge0[2] * uv1[1]) - (edge1[2] * uv0[1])) * r
        );
  
    // -- overwritting values --
    verts[idx0 +  3] = norm[0];
    verts[idx0 +  4] = norm[1];
    verts[idx0 +  5] = norm[2];
    verts[idx0 +  8] = tan[0]; 
    verts[idx0 +  9] = tan[1]; 
    verts[idx0 + 10] = tan[2]; 
  }
}


