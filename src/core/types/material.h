#ifndef MATERIAL_H
#define MATERIAL_H

#include "global/global.h"
#include "math/math_inc.h"
#include "data/material_template.h"

typedef struct material_t
{
  int albedo;    // index for assetm 
  rgbf tint;
  
  int normal;    // index for assetm 
  
  int roughness; // index for assetm 
  f32 roughness_f;
  
  int metallic;  // index for assetm 
  f32 metallic_f;

  int shader; // @TODO: add shaders to assetm, not all, have variable shaders in deferred pass

  f32  tile_scl;    // getts mul with tile
  bool tile_by_scl; // scale uv by ent scl
  vec2 tile;        // tile/scale uv coords

}material_t;

material_t material_load_from_template(const material_template_t* m);
material_t material_make_basic(int albedo, int normal, int roughness, int metallic, rgbf tint, float roughness_f, float metallic_f, int shader);

material_t material_make(int albedo, int normal, int roughness, int metallic, rgbf tint, float roughness_f, float metallic_f, int shader, f32 tile_scl, bool tile_by_scl, vec2 tile);
#endif
