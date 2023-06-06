#ifndef CORE_TRAIL_H
#define CORE_TRAIL_H

#include "global/global.h"
#include "math/math_inc.h"
#include "core/types/mesh.h"

typedef struct
{
  mesh_t mesh;
  f32*   pos_ptr;       // pointer for vec3 current position
  int    subdivisions;  // divisions in length of 'quad'
  f32    max_length;    // maybe shorter, never longer
  f32    speed;         // speed of subdivs moving towards pos_ptr

  vec3*  mid_points;    // used for calculations

}trail_t;

trail_t trail_make(f32* pos_ptr, int subdivisions, f32 max_length, f32 speed)
{
  trail_t t;
  t.pos_ptr      = pos_ptr;
  t.subdivisions = subdivisions;
  t.max_length   = max_length;
  t.speed        = speed;

  // - make mesh -
  int  verts_len = 4 + (subdivisions * 2);
  f32* verts;
  MALLOC(verts, verts_len * sizeof(f32));

  // trail of positions in the middle of the 'quad'
  int   total_points = subdivisions +2;
  t.mid_points   = MALLOC(total_points * sizeof(vec3));


  mesh_make(verts, verts_len, t.mesh);
  FREE(mid_points);
  FREE(verts);

  return t;
}

#endif // CORE_TRAIL_H
