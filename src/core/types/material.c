#include "core/types/material.h"
#include "core/assetm.h"



material_t material_load_from_template(const material_template_t* m)
{
  int albedo = -1; 
  int normal = -1; 
  int roughn = -1; 
  int metall = -1;
  int shader = -1;

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
  
  return material_make(albedo, normal, roughn, metall, (float*)m->tint, (float)m->roughn_f, (float)m->metall_f, shader);  
}

material_t material_make(int albedo, int normal, int roughness, int metallic, rgbf tint, float roughness_f, float metallic_f, int shader)
{
  // blank 1x1px white texture if no texture given 
  if (albedo    < 0) { albedo    = assetm_get_texture_idx("#internal/blank.png", true); }
  if (normal    < 0) { normal    = assetm_get_texture_idx("#internal/blank.png", false); }
  if (roughness < 0) { roughness = assetm_get_texture_idx("#internal/blank.png", false); }
  if (metallic  < 0) { metallic  = assetm_get_texture_idx("#internal/blank.png", false); }

  material_t m;
  m.albedo = albedo;
  m.normal = normal;
  m.roughness = roughness;
  m.metallic = metallic;
  vec3_copy(tint, m.tint);
  m.roughness_f = roughness_f;
  m.metallic_f = metallic_f;

  m.shader = shader; // if -1 default will be used

  return m;
}


