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

// @DOC: load material from template in data/material_template.h
material_t material_load_from_template(const material_template_t* m);
// @DOC: make material, sets tile automatic
//       albedo:      idx for texture in assetm
//       normal:      idx for texture in assetm
//       roughness:   idx for texture in assetm
//       metallic:    idx for texture in assetm
//       tint:        tint color for material
//       roughness_f: mult for roughness
//       metallic_f:  mult for metalness
//       shader:      idx for shader in assetm
material_t material_make_basic(int albedo, int normal, int roughness, int metallic, rgbf tint, f32 roughness_f, f32 metallic_f, int shader);
// @DOC: make material
//       albedo:      idx for texture in assetm
//       normal:      idx for texture in assetm
//       roughness:   idx for texture in assetm
//       metallic:    idx for texture in assetm
//       tint:        tint color for material
//       roughness_f: mult for roughness
//       metallic_f:  mult for metalness
//       shader:      idx for shader in assetm
//       tile_scl:    mult for uv in shader, i.e. tile
//       tile_by_scl: sets tile_scl to object scl
//       tile:        overall tile, i.e. (2,1) you can see the material 2 times in x but only once on y 
material_t material_make(int albedo, int normal, int roughness, int metallic, rgbf tint, f32 roughness_f, f32 metallic_f, int shader, f32 tile_scl, bool tile_by_scl, vec2 tile);
#endif
