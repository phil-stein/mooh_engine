#ifndef CORE_RENDERER_H
#define CORE_RENDERER_H

#include "global/global.h"
#include "core/types/types.h"
#include "core/terrain.h"
#include "math/math_inc.h"


// @DOC: draws the mesh_t's mesh, whether its indexed or not
//       _mesh: mesh_t* to be drawn
#define DRAW_MESH(_mesh)                                                                \
      {                                                                                 \
        _glBindVertexArray(_mesh->vao);                                                 \
        if (_mesh->indexed)                                                             \
        { _glDrawElements(GL_TRIANGLES, _mesh->indices_count, GL_UNSIGNED_INT, 0); }    \
        else                                                                            \
        { _glDrawArrays(GL_TRIANGLES, 0, _mesh->verts_count); }                         \
      }

// @DOC: initialize renderer, call this before any other calls to renderer
void renderer_init();
// @DOC: render the scene
void renderer_update();

// @DOC: draw terrain chunk using the right shader, etc.
//       view:  view matrix, instead of cams
//       proj:  projection matrix, instead of cams
//       chunk: terrain chunk
void renderer_draw_terrain(mat4 view, mat4 proj, terrain_chunk_t* chunk);
// @DOC: draw terrain chunk using no shader, just draw call, for shadow map
//       chunk: terrain chunk
void renderer_draw_terrain_mesh(terrain_chunk_t* chunk); 


#endif
