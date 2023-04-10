#include "data/entity_template.h"
#include "data/entity_tags.h"
#include "data/test_comp.h"


const entity_template_t entity_template_empty = ENTITY_TEMPLATE_T_EMPTY();

const entity_template_t entity_template_table[] = 
{
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "quad",
    .mesh = "quad",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "sphere01",
    .mesh = "sphere",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .phys_flag = ENTITY_HAS_SPHERE,
    .radius    = 1.0f,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "sphere02",
    .mesh = "sphere",
    .mat  = MATERIAL_TEMPLATE_METALL,
    .phys_flag = ENTITY_HAS_SPHERE,
    .radius    = 1.0f,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "demon01",
    .mesh = "demon01",
    .mat  = MATERIAL_TEMPLATE_DEMON01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "shotgun",
    .mesh = "shotgun",
    .mat  = MATERIAL_TEMPLATE_SHOTGUN,
    .update_f = entity_update,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "demon02",
    .mesh = "demon02",
    .mat  = MATERIAL_TEMPLATE_DEMON02,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "stone01",
    .mesh = "stones/stone01_ld02_tri",
    .mat  = MATERIAL_TEMPLATE_STONE01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "tree01",
    .mesh = "trees/tree01_ld_tri",
    .mat  = MATERIAL_TEMPLATE_TREE01,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "hut_test",
    .mesh = "base_hut",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "cube_static",
    .mesh = "cube",
    .mat  = MATERIAL_TEMPLATE_BRICK, // _PLANKS, // _PATH, // _METALL,
    .phys_flag = ENTITY_HAS_BOX,
    .aabb_size  = { 1, 1, 1 },
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "cube_dynamic",
    .tags_flag = TAG_CUBE_DYN, 
    .mesh = "cube",
    .mat  = MATERIAL_TEMPLATE_PLANKS,
    .phys_flag = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_BOX,
    .mass = 1.0f,
    .aabb_size  = { 1, 1, 1 },
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "player_test",
    .tags_flag = TAG_PLAYER,
    .mesh = "demon02",
    .mat  = MATERIAL_TEMPLATE_DEMON02,
    .init_f      = player_init,
    .update_f    = player_update,
    .collision_f = player_on_collision,
    .trigger_f   = player_on_trigger,
    .phys_flag   = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_BOX,
    .mass = 5.0f,
    .friction = 0.05f, 
    .aabb_size   = { 0.5f, 2.0f, 0.5f },
    .collider_offset = { 0.0f, 2.15f, 0.0f },
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "cube_static_trigger",
    .tags_flag = TAG_UP_FORCE,
    .mesh = "cube",
    .mat  = MATERIAL_TEMPLATE_METALL,
    .phys_flag = ENTITY_HAS_BOX,
    .aabb_size  = { 1, 1, 1 },
    .is_trigger = true,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "devil_paladin_body",
    .mesh = "mooh_body01",
    .mat  = MATERIAL_TEMPLATE_PALADIN_BODY,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "devil_paladin_armour",
    .mesh = "mooh_armour01",
    .mat  = MATERIAL_TEMPLATE_PALADIN_ARMOUR,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "devil_paladin_weapon",
    .mesh = "mooh_weapon01",
    .mat  = MATERIAL_TEMPLATE_PALADIN_WEAPON,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "sphere_dyn01",
    .mesh = "sphere",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    .phys_flag = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_SPHERE,
    .mass = 2.0f,
    .friction = 0.05f, 
    .radius    = 1.0f,
  },
  {
    ENTITY_TEMPLATE_T_SET_DEFAULTS(),
    .name = "projectile01",
    .mesh = "sphere",
    .mat  = MATERIAL_TEMPLATE_DEFAULT,
    // @TODO: .scl  = { 0.5f, 0.5f, 0.5f },
    .init_f    = projectile_init,
    .update_f  = projectile_update,
    .cleanup_f = projectile_cleanup,
    .phys_flag = ENTITY_HAS_RIGIDBODY | ENTITY_HAS_SPHERE,
    .mass = 2.0f,
    .friction = 0.05f, 
    .radius    = 1.0f,
  },
};

const entity_template_t* entity_template_get(int idx)
{
  // P_INT(idx);

  if (idx == -1)  // empty entity template
  {
    return &entity_template_empty;
  }

  ERR_CHECK(idx > -1, "entity template requested not valid: %d\n", idx); // @TODO: check upper bound
  
  return &entity_template_table[idx];
}
const entity_template_t* entity_template_get_all(int* len)
{
  *len = sizeof(entity_template_table) / sizeof(entity_template_table[0]);
  return entity_template_table;
}

