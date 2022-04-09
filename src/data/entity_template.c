#include "data/entity_template.h"
#include "data/test_comp.h"


const template_t template_table[] = 
{
  {
    .name = "quad",
    .mesh = "quad.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "sphere01",
    .mesh = "sphere.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "sphere02",
    .mesh = "sphere.fbx",
    .mat  = MATERIAL_TEMPLATE_METALL,
    .init   = NULL,
    .update = NULL,
    .phys_flags = ENTITY_HAS_RIGIDBODY,
    .mass = 1.0f,
  },
  {
    .name = "demon01",
    .mesh = "demon01.fbx",
    .mat  = MATERIAL_TEMPLATE_DEMON01,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "shotgun",
    .mesh = "shotgun.fbx",
    .mat  = MATERIAL_TEMPLATE_SHOTGUN,
    .init   = NULL,
    .update = entity_update,
    .phys_flags = 0,
  },
  {
    .name = "demon02",
    .mesh = "demon02.fbx",
    .mat  = MATERIAL_TEMPLATE_DEMON02,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "stone01",
    .mesh = "stones/stone01_ld02_tri.fbx",
    .mat  = MATERIAL_TEMPLATE_STONE01,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "tree01",
    .mesh = "trees/tree01_ld_tri.fbx",
    .mat  = MATERIAL_TEMPLATE_TREE01,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "hut_test",
    .mesh = "base_hut.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .init   = NULL,
    .update = NULL,
    .phys_flags = 0, 
  },
  {
    .name = "cube_static",
    .mesh = "cube.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .init   = NULL,
    .update = NULL,
    .phys_flags = ENTITY_HAS_BOX,
    .aabb_size  = { 1, 1, 1 },
  },
  {
    .name = "cube_dynamic",
    .mesh = "cube.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .init   = NULL,
    .update = NULL,
    .phys_flags = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_BOX,
    .mass = 1.0f,
    .aabb_size  = { 1, 1, 1 },
  },
};

const template_t* template_get(int idx)
{
  return &template_table[idx];
}
const template_t* template_get_all(int* len)
{
  *len = sizeof(template_table) / sizeof(template_table[0]);
  return template_table;
}

