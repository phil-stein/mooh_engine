#ifndef RENDERER_H
#define RENDERER_H

#include "global/global.h"
#include "core/types/types.h"
#include "core/terrain.h"
#include "math/math_inc.h"


#define DRAW_MESH(_mesh)                                                                \
      {                                                                                 \
        glBindVertexArray(_mesh->vao);                                                  \
        if (_mesh->indexed)                                                             \
        { glDrawElements(GL_TRIANGLES, _mesh->indices_count, GL_UNSIGNED_INT, 0); }     \
        else                                                                            \
        { glDrawArrays(GL_TRIANGLES, 0, _mesh->verts_count); }                          \
      }

void renderer_init();
void renderer_update();

void renderer_draw_terrain(mat4 view, mat4 proj, terrain_chunk_t* chunk);
void renderer_draw_terrain_mesh(terrain_chunk_t* chunk); // just the draw-call, no shader calls, used for shadow-map


#endif
