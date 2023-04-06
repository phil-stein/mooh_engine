#include "core/types/material.h"
#include "core/io/assetm.h"
#include "core/debug/debug_timer.h"



material_t material_load_from_template(const material_template_t* m, material_template_type idx)
{
  int albedo = -1; 
  int normal = -1; 
  int roughn = -1; 
  int metall = -1;
  int shader = -1;
  // vec2 tile = VEC2_INIT(1);

  if (m->albedo != NULL) 
  { albedo = assetm_get_texture_idx(m->albedo, true); }
  if (m->normal != NULL) 
  { normal = assetm_get_texture_idx(m->normal, false); }
  if (m->roughn != NULL) 
  { roughn = assetm_get_texture_idx(m->roughn, false); }
  if (m->metall != NULL) 
  { metall = assetm_get_texture_idx(m->metall, false); }

  if (m->shader_template != SHADER_TEMPLATE_NONE)
  { shader = assetm_get_shader_idx(m->shader_template); }

  return material_make(albedo, normal, roughn, metall, (f32*)m->tint, (f32)m->roughn_f, (f32)m->metall_f, shader, m->tile_scl, m->tile_by_scl, (f32*)m->tile, idx);  
}


material_t material_make_basic(int albedo, int normal, int roughness, int metallic, rgbf tint, f32 roughness_f, f32 metallic_f, int shader)
{

  return material_make(albedo, normal, roughness, metallic, tint, roughness_f, metallic_f, shader, 1.0f, false, VEC2(1), -1);
}

material_t material_make(int albedo, int normal, int roughness, int metallic, rgbf tint, f32 roughness_f, f32 metallic_f, int shader, f32 tile_scl, bool tile_by_scl, vec2 tile, material_template_type template_idx)
{
  // blank 1x1px white texture if no texture given 
  if (albedo    < 0) { albedo    = assetm_get_texture_idx("#internal/blank.png", true); }
  if (normal    < 0) { normal    = assetm_get_texture_idx("#internal/blank.png", false); }
  if (roughness < 0) { roughness = assetm_get_texture_idx("#internal/blank.png", false); }
  if (metallic  < 0) { metallic  = assetm_get_texture_idx("#internal/blank.png", false); }

  material_t m;
  m.template_idx = template_idx;
  m.albedo = albedo;
  m.normal = normal;
  m.roughness = roughness;
  m.metallic = metallic;
  vec3_copy(tint, m.tint);
  m.roughness_f = roughness_f;
  m.metallic_f = metallic_f;

  m.shader = shader; // if -1 default will be used
 
  m.tile_scl = tile_scl;
  m.tile_by_scl = tile_by_scl;
  // @TODO: pass as arg
  vec2_copy(tile, m.tile);

  return m;
}


