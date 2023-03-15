#include "core/types/mesh.h"
#include "math/math_inc.h"

#include "GLAD/glad.h"
#include "ufbx/ufbx.h"
#include "stb/stb_ds.h"         


void mesh_make(float* verts, int verts_len, mesh_t* m)
{
	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vbo);
	glBindVertexArray(m->vao);

	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, verts_len * sizeof(float), verts, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
  //     FLOATS_PER_VERT * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
  //     FLOATS_PER_VERT * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
  //     FLOATS_PER_VERT * sizeof(float), (void*)(6 * sizeof(float)));
	// glEnableVertexAttribArray(2);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
  // uv's
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// tangents
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  m->floats_per_vert = FLOATS_PER_VERT;
  m->verts_count = verts_len / m->floats_per_vert;
  m->indices_count = 0;

  m->indexed = false;
  m->loaded = true;
}

void mesh_make_indexed(float* verts, int verts_len, unsigned int* indices, int indices_len, mesh_t* m)
{

  glGenVertexArrays(1, &m->vao);
  glGenBuffers(1, &m->vbo);
  glGenBuffers(1, &m->ebo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(m->vao);

  glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
  glBufferData(GL_ARRAY_BUFFER, verts_len * sizeof(f32), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_len * sizeof(u32), indices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
  // uv's
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// tangents
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 
      FLOATS_PER_VERT * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  m->floats_per_vert = FLOATS_PER_VERT;
  m->verts_count = verts_len / m->floats_per_vert;
  m->indices_count = indices_len;

  m->indexed = true;
  m->loaded  = true;

}

void mesh_free(mesh_t* m)
{
  if (!m->loaded) { return; }
  glDeleteBuffers(1, &m->vao);
  glDeleteBuffers(1, &m->vbo);
  if (m->indexed)
  { glDeleteBuffers(1, &m->ebo); }
}

// -- ufbx --
mesh_t mesh_load(const char* path)
{
  ERR("refactor this to use mesh_load_from_memory()");
  /*
  ufbx_load_opts opts = { NULL }; // Optional, pass NULL for defaults
  ufbx_error error; // Optional, pass NULL if you don't care about errors
  ufbx_scene *scene = ufbx_load_file(path, &opts, &error);
  if (!scene) { P("UFBX had an error.\nexiting ..."); exit(1); }

  // Use and inspect `scene`, it's just plain data!

  // Geometry is always stored in a consistent indexed format:
  float* verts = NULL;
  u32*   indices = NULL;
  // ufbx_mesh* cube = ufbx_find_mesh(scene, "Cube");
  assert(scene->meshes.size > 0);
  ufbx_mesh* m = &scene->meshes.data[0];

  // triangulate
  // u32 tris[6];
  // ufbx_face face = m->faces[0];
  // bool err = ufbx_triangulate(tris, 6, m, face);
  // assert(err);

  
  for (size_t face_ix = 0; face_ix < m->num_faces; face_ix++) 
  {
    ufbx_face face = m->faces[face_ix];
    // assert(ufbx_triangulate(tris, 6, m, face));
    
    for (size_t vertex_ix = 0; vertex_ix < face.num_indices; vertex_ix++) 
    {
      size_t index = face.index_begin + vertex_ix;
      arrput(indices, (u32)index);
      ufbx_vec3 pos    = m->vertex_position.data[m->vertex_position.indices[index]];
      ufbx_vec3 normal = ufbx_get_vertex_vec3(&m->vertex_normal, index);
      ufbx_vec2 uv     = m->vertex_uv.data[m->vertex_uv.indices[index]];

      // arrput(verts, pos.v[0]);
      // arrput(verts, pos.v[1]);
      // arrput(verts, pos.v[2]);
      // arrput(verts, normal.v[0]);
      // arrput(verts, normal.v[1]);
      // arrput(verts, normal.v[2]);
      // arrput(verts, uv.v[0]);
      // arrput(verts, uv.v[1]);
      
      // @NOTE: flip to go from blender coord sys to the engines
      arrput(verts, pos.v[0]);
      arrput(verts, pos.v[2]);
      arrput(verts, -pos.v[1]);
      arrput(verts, normal.v[0]);
      arrput(verts, normal.v[2]);
      arrput(verts, -normal.v[1]);
      arrput(verts, uv.v[0]);
      arrput(verts, uv.v[1]);
      arrput(verts, tan[0]);
      arrput(verts, tan[2]);
      arrput(verts, -tan[1]);

    }
  }

  ufbx_free_scene(scene);
  */
  mesh_t mesh;
  // mesh_make_indexed(verts, arrlen(verts), indices, arrlen(indices), &mesh);
  // arrfree(verts);
  // arrfree(indices);
  return mesh;

}
mesh_t mesh_load_from_memory(const void* data, size_t size, const char* name)
{
  f32* verts   = NULL;
  u32* indices = NULL; 
  mesh_load_data_from_memory(data,  size, name, &verts, &indices);
  
  mesh_t mesh;
  mesh_make_indexed(verts, arrlen(verts), indices, arrlen(indices), &mesh);
  ARRFREE(verts);
  ARRFREE(indices);

  return mesh;
}
void mesh_load_data_from_memory(const void* data, size_t size, const char* name, f32** verts, u32** indices)
{
  ufbx_load_opts opts = { NULL }; // Optional, pass NULL for defaults
  ufbx_error error; // Optional, pass NULL if you don't care about errors
  ufbx_scene *scene = ufbx_load_memory(data, size, &opts, &error);
  if (!scene) { ERR("UFBX had an error '%s'\n", name); }

  // Use and inspect `scene`, it's just plain data!

  // Geometry is always stored in a consistent indexed format:

  // ufbx_mesh* cube = ufbx_find_mesh(scene, "Cube");
  assert(scene->meshes.size > 0);
  ufbx_mesh* m = &scene->meshes.data[0];

  for (size_t face_ix = 0; face_ix < m->num_faces; face_ix++) 
  {
    ufbx_face face = m->faces[face_ix];
    ERR_CHECK(face.num_indices == 3, "mesh '%s' has faces with more than 3 triangles.\n", name); 
   
    // calculating tangents based on: https://marti.works/posts/post-calculating-tangents-for-your-mesh/post/
    
    int i0 = face.index_begin;
    int i1 = i0 +1;
    int i2 = i0 +2;

    ufbx_vec3 _pos0 = m->vertex_position.data[m->vertex_position.indices[i0]];
    ufbx_vec3 _pos1 = m->vertex_position.data[m->vertex_position.indices[i1]];
    ufbx_vec3 _pos2 = m->vertex_position.data[m->vertex_position.indices[i2]];
    vec3 pos0 = { _pos0.x, _pos0.y, _pos0.z };
    vec3 pos1 = { _pos1.x, _pos1.y, _pos1.z };
    vec3 pos2 = { _pos2.x, _pos2.y, _pos2.z };
    // vec3 pos0 = { _pos0.x, _pos0.z, _pos0.y };
    // vec3 pos1 = { _pos1.x, _pos1.z, _pos1.y };
    // vec3 pos2 = { _pos2.x, _pos2.z, _pos2.y };

    ufbx_vec2 _tex0 = m->vertex_uv.data[m->vertex_uv.indices[i0]];
    ufbx_vec2 _tex1 = m->vertex_uv.data[m->vertex_uv.indices[i1]];
    ufbx_vec2 _tex2 = m->vertex_uv.data[m->vertex_uv.indices[i2]];
    vec2 tex0 = { _tex0.x, _tex0.y };
    vec3 tex1 = { _tex1.x, _tex1.y };
    vec3 tex2 = { _tex2.x, _tex2.y };
    // P("-- mesh --");
    // P_VEC2(tex0);
    // P_VEC2(tex1);
    // P_VEC2(tex2);
   
    vec2 uv0, uv1;
    vec2_sub(tex1, tex0, uv0);
    vec2_sub(tex2, tex0, uv1);
    // P_VEC2(uv0);
    // P_VEC2(uv1);
    
    vec3 edge0, edge1;
    vec3_sub(pos1, pos0, edge0);
    vec3_sub(pos2, pos0, edge1);
    // P_VEC3(edge0);
    // P_VEC3(edge1);

    float r = 1.0f / (uv0[0] * uv1[1] - uv0[1] * uv1[0]);
    // P_F32(r);

    vec3 tan = VEC3_XYZ_INIT(
        ((edge0[0] * uv1[1]) - (edge1[0] * uv0[1])) * r,
        ((edge0[1] * uv1[1]) - (edge1[1] * uv0[1])) * r,
        ((edge0[2] * uv1[1]) - (edge1[2] * uv0[1])) * r
        );
    // P_VEC3(tan);


    for (size_t vertex_ix = 0; vertex_ix < face.num_indices; vertex_ix++) 
    {
      size_t index = face.index_begin + vertex_ix;
      arrput((*indices), (u32)index);
      ufbx_vec3 pos    = m->vertex_position.data[m->vertex_position.indices[index]];
      ufbx_vec3 normal = ufbx_get_vertex_vec3(&m->vertex_normal, index);
      ufbx_vec2 uv     = m->vertex_uv.data[m->vertex_uv.indices[index]];
     
      // @NOTE: flip to go from blender coord sys to the engines
      // arrput(verts, pos.v[0]);
      // arrput(verts, pos.v[1]);
      // arrput(verts, pos.v[2]);
      // arrput(verts, normal.v[0]);
      // arrput(verts, normal.v[1]);
      // arrput(verts, normal.v[2]);
      // arrput(verts, uv.v[0]);
      // arrput(verts, uv.v[1]);
      // arrput(verts, tan[0]);
      // arrput(verts, tan[1]);
      // arrput(verts, tan[2]);
      arrput((*verts), pos.v[0]);
      arrput((*verts), pos.v[2]);
      arrput((*verts), -pos.v[1]);
      arrput((*verts), normal.v[0]);
      arrput((*verts), normal.v[2]);
      arrput((*verts), -normal.v[1]);
      arrput((*verts), uv.v[0]);
      arrput((*verts), uv.v[1]);
      arrput((*verts), tan[0]);
      arrput((*verts), tan[2]);
      arrput((*verts), -tan[1]);
    }
  }

  ufbx_free_scene(scene);
}
