#ifndef MESH_H
#define MESH_H

#include "global/global.h"

#define FLOATS_PER_VERT (3 + 3 + 2 + 3)  // pos norm uv tan

typedef struct mesh_t
{
  bool loaded;
  bool indexed;
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;
  int floats_per_vert;
  int verts_count;
  int indices_count;

}mesh_t;

void mesh_make(float* verts, int verts_len, mesh_t* m);
void mesh_make_indexed(float* verts, int verts_len, unsigned int* indices, int indices_len, mesh_t* m);
void mesh_free(mesh_t* m);

// load mesh from file
mesh_t mesh_load(const char* file_path);
// load mesh from buffer in memory, name for debug
mesh_t mesh_load_from_memory(const void* data, size_t size, const char* name);

#endif
