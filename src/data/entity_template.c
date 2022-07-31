#include "data/entity_template.h"
#include "data/test_comp.h"


const entity_template_t entity_template_table[] = 
{
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "quad",
    .mesh = "quad.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "sphere01",
    .mesh = "sphere.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "sphere02",
    .mesh = "sphere.fbx",
    .mat  = MATERIAL_TEMPLATE_METALL,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "demon01",
    .mesh = "demon01.fbx",
    .mat  = MATERIAL_TEMPLATE_DEMON01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "shotgun",
    .mesh = "shotgun.fbx",
    .mat  = MATERIAL_TEMPLATE_SHOTGUN,
    .update = entity_update,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "demon02",
    .mesh = "demon02.fbx",
    .mat  = MATERIAL_TEMPLATE_DEMON02,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "stone01",
    .mesh = "stones/stone01_ld02_tri.fbx",
    .mat  = MATERIAL_TEMPLATE_STONE01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "tree01",
    .mesh = "trees/tree01_ld_tri.fbx",
    .mat  = MATERIAL_TEMPLATE_TREE01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "hut_test",
    .mesh = "base_hut.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "cube_static",
    .mesh = "cube.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .phys_flags = ENTITY_HAS_BOX,
    .aabb_size  = { 1, 1, 1 },
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS()
    .name = "cube_dynamic",
    .mesh = "cube.fbx",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .phys_flags = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_BOX,
    .mass = 1.0f,
    .aabb_size  = { 1, 1, 1 },
  },
  {
    .name = "player_test",
    .mesh = "demon02.fbx",
    .mat  = MATERIAL_TEMPLATE_DEMON02,
    .init   = player_init,
    .update = player_update,
    .phys_flags = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_BOX,
    .mass = 5.0f,
    .aabb_size   = { 0.5f, 2.0f, 0.5f },
    .aabb_offset = { 0.0f, 2.15f, 0.0f },
  },
};

const entity_template_t* entity_template_get(int idx)
{
  return &entity_template_table[idx];
}
const entity_template_t* entity_template_get_all(int* len)
{
  *len = sizeof(entity_template_table) / sizeof(entity_template_table[0]);
  return entity_template_table;
}

