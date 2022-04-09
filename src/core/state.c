#include "core/state.h"
#include "core/assetm.h"
#include "data/entity_template.h"
#include "math/math_inc.h"
#include "phys/phys_world.h"

#include "stb/stb_ds.h"


entity_t* world = NULL;
int       world_len = 0;

int* world_dead = NULL;
int  world_dead_len = 0;

dir_light_t dir_lights[DIR_LIGHTS_MAX];
int          dir_lights_len = 0;

point_light_t point_lights[POINT_LIGHTS_MAX];
int           point_lights_len = 0;


void state_init()
{
  for (int i = 0; i < world_len; ++i)
  {
    if (world[i].init_f != NULL)
    { world[i].init_f(&world[i]); }
  }
}

void state_update(float dt)
{
  for (int i = 0; i < world_len; ++i)
  {
    state_entity_update_global_model(i);
    
    if (world[i].update_f != NULL)
    { world[i].update_f(&world[i], dt); }
  }
}

entity_t* state_get_entity_arr(int* len)
{
  *len = world_len;
  return world;
}

int state_add_entity_from_template(vec3 pos, vec3 rot, vec3 scl, int idx)
{
  const template_t* def = template_get(idx);
  int mesh = assetm_get_mesh_idx(def->mesh);
  int mat  = assetm_get_material_idx(def->mat);


  int id = state_add_entity(pos, rot, scl, mesh, mat, def->init, def->update, idx);

  if (HAS_FLAG(def->phys_flags, ENTITY_HAS_BOX) && HAS_FLAG(def->phys_flags, ENTITY_HAS_RIGIDBODY))
  {
    vec3 half_extents;
    vec3 aabb[2];
    vec3_mul_f((f32*)def->aabb_size, 1, half_extents);
    vec3_copy(half_extents, aabb[0]);
    vec3_copy(half_extents, aabb[1]);
    vec3_mul_f(aabb[0], -1, aabb[0]);
    
    phys_add_obj_rb_box(id, pos, scl, def->mass, aabb, VEC3(0));
  }
  else if (HAS_FLAG(def->phys_flags, ENTITY_HAS_RIGIDBODY))
  {
    phys_add_obj_rb(id, pos, def->mass);
  }
  else if (HAS_FLAG(def->phys_flags, ENTITY_HAS_BOX))
  {
    vec3 half_extents;
    vec3 aabb[2];
    vec3_mul_f((f32*)def->aabb_size, 1, half_extents);
    vec3_copy(half_extents, aabb[0]);
    vec3_copy(half_extents, aabb[1]);
    vec3_mul_f(aabb[0], -1, aabb[0]);
    
    phys_add_obj_box(id, pos, scl, aabb, VEC3(0));
  }

  return id; 
}

int state_add_entity(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, init_callback* init_f, update_callback* update_f, int table_idx)
{
  entity_t ent;
  ent.is_dead = false;
  ent.table_idx = table_idx;
  
  // not using 'ENTITY_SET_...',  as already setting 'is_moved'
  vec3_copy(pos,  ent.pos);
  vec3_copy(rot,  ent.rot);
  vec3_copy(scl,  ent.scl);
  ent.is_moved     = true; 
  ent.mesh         = mesh;
  ent.mat          = mat;
  ent.init_f       = init_f;
  ent.update_f     = update_f;
  ent.children     = NULL;
  ent.children_len = 0;
  ent.parent       = -1;

  int idx = -1;
  // check for free slot
  if (world_dead_len > 0)
  {
    idx = arrpop(world_dead);
    ent.id = idx;
    world_dead_len--;
    world[idx] = ent;
  }
  else
  {
    idx = world_len;
    ent.id = idx;
    arrput(world, ent);
    world_len++;
  }

  return idx;
}
int state_duplicate_entity(int id, vec3 offset)
{
  bool err = false;
  entity_t* e = state_get_entity(id, &err); assert(!err);
  vec3 pos;
  vec3_add(e->pos, offset, pos);
  return state_add_entity_from_template(pos, e->rot, e->scl, e->table_idx);
  // return state_add_entity(pos, e->rot, e->scl, e->mesh, e->mat, e->init_f, e->update_f, e->table_idx);
}
void state_remove_entity(int id)
{
  ERR_CHECK(id >= 0 && id < world_len, "invalid entity id");
 
  if (world[id].parent > -1)
  {
    state_entity_remove_child(world[id].parent, id);
  }
  for (int i = 0; i < world[id].children_len; ++i)
  {
    state_entity_remove_child(world[id].id, world[id].children[i]);
  }

  world[id].is_dead = true;
  arrput(world_dead, id);
  world_dead_len++;
}
// entity_t* state_get_entity(int id, bool* error)
entity_t* state_get_entity_dbg(int id, bool* error, char* file, int line)
{
  // ERR_CHECK(id >= 0 && id < world_len, "invalid entity id: %d, [file: %s, line: %d]", id, file, line);
  // ERR_CHECK(!world[id].is_dead, "requested dead entity: %d, [file: %s, line: %d]", id, file, line);
  *error = id < 0 || id > world_len-1 || world[id].is_dead;
  return &world[id];
}
void state_entity_add_child(int parent, int child)
{
  if (parent < 0 || child < 0 || parent >= world_len || child >= world_len) 
  {
    P_ERR("parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); 
    return;
  }
  
  bool error = false;
  entity_t* p = state_get_entity(parent, &error); assert(!error);
  entity_t* c = state_get_entity(child, &error); assert(!error);

  if (c->parent > -1) 
  { P_ERR("parenting child which is already parented."); return; } 

  arrput(p->children, child);
  p->children_len++;
  c->parent = parent;
  PF("-> parent: %d, child: %d\n", p->id, c->id);
}
void state_entity_remove_child(int parent, int child)
{
  if (parent < 0 || child < 0 || parent >= world_len || child >= world_len) 
  { 
    P_ERR("un-parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); 
    return;
  }
  
  bool error = false;
  entity_t* p = state_get_entity(parent, &error); assert(!error);
  entity_t* c = state_get_entity(child, &error); assert(!error);

  for (int i = 0; i < p->children_len; ++i)
  {
    if (p->children[i] == child)
    {
      arrdel(p->children, i);
      p->children_len--;
    }
  }
  c->parent = -1;
}
void state_entity_local_model(int id, mat4 out)
{  
  if (id < 0 || id >= world_len) 
  { 
    P_ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);
  mat4_make_model(e->pos, e->rot, e->scl, out);
}
void state_entity_update_global_model(int id)
{
  if (id < 0 || id >= world_len) 
  { 
    ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);

  if (!e->is_moved) { return; }

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, e->rot, e->scl, e->model);
    entity_t* p = state_get_entity(e->parent, &error); assert(!error);
    mat4_mul(p->model, e->model, e->model);
  }
  else
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, e->rot, e->scl, e->model);
  }
  for (int i = 0; i < e->children_len; ++i)
  {
    state_entity_update_global_model(e->children[i]);
  }
  e->is_moved = false;
}
void state_entity_global_model_no_rotation(int id, mat4 out)
{
  if (id < 0 || id >= world_len) 
  { 
    P_ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), e->scl, out);
    entity_t* p = state_get_entity(e->parent, &error); assert(!error);
    mat4_mul(p->model, e->model, out);
  }
  else
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), e->scl, out);
  }
}
void state_entity_model_no_scale(int id, mat4 out)
{
  if (id < 0 || id >= world_len) 
  { 
    P_ERR("no scale model with invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    // mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    mat4_make_model(e->pos, e->rot, VEC3(1), out);
    entity_t* p = state_get_entity(e->parent, &error); assert(!error);
    mat4_mul(p->model, out, out);
  }
  else
  {
    // same as state_entity_local_model()
    // mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    mat4_make_model(e->pos, e->rot, VEC3(1), out);
  }
}
void state_entity_model_no_scale_rotation(int id, mat4 out)
{
  if (id < 0 || id >= world_len) 
  { 
    P_ERR("no scale model with invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    entity_t* p = state_get_entity(e->parent, &error); assert(!error);
    mat4_mul(p->model, out, out);
  }
  else
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
  }
}
void state_entity_global_scale(int id, vec3 out)
{
  if (id < 0 || id >= world_len) 
  { 
    P_ERR("invalid id. id'%d'", id); 
    return;
  }
  
  bool error = false;
  entity_t* e = state_get_entity(id, &error); assert(!error);

  if (e->parent <= -1) { vec3_copy(e->scl, out); return; }
  
  state_entity_global_scale(e->parent, out);
  vec3_mul(out, e->scl, out);
}

dir_light_t* state_get_dir_light_arr(int* len)
{
  *len = dir_lights_len;
  return dir_lights;
}

bool state_add_dir_light(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y)
{
  if (dir_lights_len >= DIR_LIGHTS_MAX -1) { return false; }
  
  dir_light_t l;
  vec3_copy(pos, l.pos);
  vec3_copy(dir, l.dir);
  vec3_copy(color, l.color);
  // vec3_copy(ambient, l.ambient);
  // vec3_copy(diffuse, l.diffuse);
  l.intensity    = intensity;
  l.cast_shadow  = cast_shadow;
  l.shadow_map_x = shadow_map_x;
  l.shadow_map_y = shadow_map_y;
  if (cast_shadow)
  { framebuffer_create_shadowmap(&l.fb_shadow.buffer, &l.fb_shadow.fbo, l.shadow_map_x, l.shadow_map_y); }
  
  dir_lights[dir_lights_len++] = l;
  return true;
}

point_light_t* state_get_point_light_arr(int* len)
{
  *len = point_lights_len;
  return point_lights;
}

bool state_add_point_light(vec3 pos, rgbf color, float intensity)
{
  if (dir_lights_len >= POINT_LIGHTS_MAX -1) { return false; }
  
  point_light_t l;
  vec3_copy(pos, l.pos);
  vec3_copy(color, l.color);
  l.intensity    = intensity;
  
  point_lights[point_lights_len++] = l;
  return true;
}
