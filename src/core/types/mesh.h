#ifndef MESH_H
#define MESH_H

#include "../../global/global.h"

// @NOTE: mesh_t also has a floats_per_vert var in case its not this
#define FLOATS_PER_VERT (3 + 3 + 2 + 3)  // pos norm uv tan

typedef struct mesh_t
{
  bool loaded;           
  bool indexed;         // whether the mesh is indexed or not, glDrawIndexed
  unsigned int vao;     // vertex array object index for opengl 
  unsigned int vbo;     // vertex buffer object index for opengl 
  unsigned int ebo;     // element buffer object index for opengl
  int floats_per_vert;  // i.e. 3 floats for pos + 3 for scale, rot, etc. normaly this is FLOATS_PER_VERT 
  int verts_count;      
  int indices_count;

}mesh_t;

// @DOC: make mesh that isn't idexed
//       verts:     array of vertices
//       verts_len: length of verts array
//       m:         pointer to mesh to be made 
void mesh_make(float* verts, int verts_len, mesh_t* m);
// @DOC: make mesh that is idexed
//       verts:       array of vertices
//       verts_len:   length of verts array
//       indices:     array of indices
//       indices_len: length of indices array
//       m:           pointer to mesh to be made 
void mesh_make_indexed(float* verts, int verts_len, unsigned int* indices, int indices_len, mesh_t* m);
// @DOC: free mesh, all alloced memory and opengl data stored on gpu
void mesh_free(mesh_t* m);

// @DOC: load mesh from file
//       file_path: path to .fbx mesh file
mesh_t mesh_load(const char* file_path);
// @DOC: load mesh from buffer in memory, data is loaded .fbx file
//       data: contents of .fbx file
//       size: length of data
//       name: name to be given the mesh
mesh_t mesh_load_from_memory(const void* data, size_t size, const char* name);
// @DOC: load mesh verts and indices from memory and put it into arrays
//       ! verts & indices have to be NULL, and freed using arrfree() afterwards
//       data:    contents of .fbx file
//       size:    length of data
//       name:    name to be given the mesh
//       verts:   ptr to array for vertices, has to be freed using arrfree()
//       indices: ptr to array for indices, has to be freed using arrfree()
void mesh_load_data_from_memory(const void* data, size_t size, const char* name, f32** verts, u32** indices);

#endif
