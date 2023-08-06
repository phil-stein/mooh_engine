#include "core/state.h"
#include "core/core_data.h"
#include "core/event_sys.h"
#include "core/io/assetm.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "data/entity_template.h"
#include "math/math_inc.h"
#include "phys/phys_world.h"

#include "stb/stb_ds.h"


entity_t* world_arr = NULL;   // all entities
int       world_arr_len = 0;

int* world_dead_arr = NULL;   // all entities marked dead, aka. ready to be overwritten
int  world_dead_arr_len = 0;

int** template_entity_idxs_arr = NULL;  // array of arrays with all entities belonging to one template arr[template][i] = i'th ent of template
int   template_entity_idxs_arr_len = 0;

dir_light_t dir_lights_arr[DIR_LIGHTS_MAX];
int         dir_lights_arr_len = 0;

point_light_t* point_lights_arr = NULL;
int            point_lights_arr_len = 0;

int* point_lights_dead_arr = NULL;
int  point_lights_dead_arr_len = 0;

// @NOTE: entity->init() get called in state_init(), in the editor they get called when play is pressed
//        also used to call init_f direct in add_entity() during game
bool entity_init_called = false;


static core_data_t* core_data = NULL;


// -- shared variables --
// shared variable dont use this just for error detection in state_entity_get(), extern def in state.h
bool __state_entity_get_error_shared = false;
int* __state_world_arr_len_ptr_shared = NULL;

void state_init()
{
  core_data = core_data_get();

  __state_world_arr_len_ptr_shared = &world_arr_len;
  
    core_data->world_arr_len_ptr      = &world_arr_len;
  core_data->world_dead_arr_len_ptr = &world_dead_arr_len;

  int templates_len = 0;
  entity_template_get_all(&templates_len);
  MALLOC(template_entity_idxs_arr, templates_len * sizeof(int*));
  template_entity_idxs_arr_len = templates_len;
  for (u32 i = 0; i < template_entity_idxs_arr_len; ++i)
  { template_entity_idxs_arr[i] = NULL; }


// gest called in state_update() in editor
#ifndef EDITOR
  state_call_entity_init();
#endif
}
void state_call_entity_init()
{
  for (int i = 0; i < world_arr_len; ++i)
  {
    if (world_arr[i].init_f != NULL)
    { world_arr[i].init_f(&world_arr[i]); }
  }
  entity_init_called = true;
}

void state_update()
{
  // @NOTE: gets called in state_init(), this only affects the editor
#ifdef EDITOR
  if (!entity_init_called && core_data->scripts_act)
  {
    state_call_entity_init();
  }
#endif

  for (int i = 0; i < world_arr_len; ++i)
  {
    if (world_arr[i].is_dead) { continue; }

    state_entity_update_global_model(&world_arr[i]);
    
    // call entity update func
    if (core_data->scripts_act && world_arr[i].update_f != NULL)
    { world_arr[i].update_f(&world_arr[i], core_data->delta_t); }

    // @NOTE: no longer works this way [12.03.23]
    // // set point light pos to entity pos
    // if (world_arr[i].point_light_idx >= 0)
    // {
    //   // vec3_copy(world_arr[i].pos, point_lights_arr[world_arr[i].point_light_idx].pos);
    // }
  }
}

void state_clear_scene()
{
  phys_clear_state();

  ARRFREE(world_arr);
  world_arr_len = 0;

  ARRFREE(world_dead_arr);
  world_dead_arr_len = 0;

  dir_lights_arr_len   = 0;
  point_lights_arr_len = 0;
}

entity_t* state_entity_get_arr(int* len, int* dead_len)
{
  *len = world_arr_len;
  *dead_len = world_dead_arr_len;
  return world_arr;
}
int** state_entity_get_template_idxs_arr(int* len)
{
  *len = template_entity_idxs_arr_len;
  return template_entity_idxs_arr;
}

int state_entity_add_from_template(vec3 pos, vec3 rot, vec3 scl, int template_idx)
{
  const entity_template_t* def = entity_template_get(template_idx);
  int mesh = -1;
  if (!(strlen(def->mesh) == 1 && def->mesh[0] == '-')) // isnt equal to "-", that means no mesh
  { mesh = assetm_get_mesh_idx(def->mesh); }
  int mat  = 0; 
  if (def->mat > -1)    // isnt -1 as thats no mat
  { mat = assetm_get_material_idx(def->mat); }

  int id = state_entity_add(pos, rot, scl, mesh, mat, def->tags_flag, def->phys_flag, def->init_f, def->update_f, def->cleanup_f, def->collision_f, def->trigger_f, template_idx);

  if (HAS_FLAG(def->phys_flag, ENTITY_HAS_BOX) && HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
  {
    vec3 half_extents;
    vec3 aabb[2];
    vec3_mul_f((f32*)def->aabb_size, 1, half_extents);
    vec3_copy(half_extents, aabb[0]);
    vec3_copy(half_extents, aabb[1]);
    vec3_mul_f(aabb[0], -1, aabb[0]);
    
    phys_add_obj_rb_box(id, pos, scl, def->mass, def->friction, aabb, (f32*)def->collider_offset, def->is_trigger);
  }
  else if (HAS_FLAG(def->phys_flag, ENTITY_HAS_SPHERE) && HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
  {
    phys_add_obj_rb_sphere(id, pos, scl, def->mass, def->friction, def->radius, (f32*)def->collider_offset, def->is_trigger);
  }
  else if (HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
  {
    phys_add_obj_rb(id, pos, def->mass, def->friction);
  }
  else if (HAS_FLAG(def->phys_flag, ENTITY_HAS_BOX))
  {
    vec3 half_extents;
    vec3 aabb[2];
    vec3_mul_f((f32*)def->aabb_size, 1, half_extents);
    vec3_copy(half_extents, aabb[0]);
    vec3_copy(half_extents, aabb[1]);
    vec3_mul_f(aabb[0], -1, aabb[0]);
    
    phys_add_obj_box(id, pos, scl, aabb, (f32*)def->collider_offset, def->is_trigger);
  }
  else if (HAS_FLAG(def->phys_flag, ENTITY_HAS_SPHERE))
  {
    phys_add_obj_sphere(id, pos, scl, def->radius, (f32*)def->collider_offset, def->is_trigger);
  }
  
  return id; 
}

int state_entity_add(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, s64 tags_flags, entity_phys_flag phys_flag, init_callback* init_f, update_callback* update_f, cleanup_callback* cleanup_f, collision_callback* collision_f, trigger_callback* trigger_f, int template_idx)
{
  entity_t ent;
  ent.is_dead = false;
  ent.template_idx = template_idx;
  ent.tags_flag = tags_flags;

  // not using 'ENTITY_SET_...',  as already setting 'is_moved'
  vec3_copy(pos,  ent.pos);
  vec3_copy(rot,  ent.rot);
  vec3_copy(scl,  ent.scl);
  ent.is_moved        = true; 
  ent.mesh[0]         = mesh;
  ent.mat[0]          = mat;
  ent.mesh_count      = 1;
  ent.point_light_idx = -1;
  ent.phys_flag       = phys_flag;
  ent.is_grounded     = false; 
  ent.init_f          = init_f;
  ent.update_f        = update_f;
  ent.cleanup_f       = cleanup_f;
  ent.collision_f     = collision_f;
  ent.trigger_f       = trigger_f;
  ent.children        = NULL;
  ent.children_len    = 0;
  ent.parent          = -1;

  int id = -1;
  // check for free slot
  if (world_dead_arr_len > 0)
  {
    id = arrpop(world_dead_arr);
    ent.id = id;
    world_dead_arr_len--;
    world_arr[id] = ent;
  }
  else
  {
    id = world_arr_len;
    ent.id = id;
    arrput(world_arr, ent);
    world_arr_len++;
  }
  
  event_sys_trigger_entity_added(id);
 
  // add to templates_entity_idxs_arr
  if (template_idx >= 0) 
  { 
    arrput(template_entity_idxs_arr[template_idx], id); 
    // template_entity_idxs_arr_len++; 
  }

  // in case adding ent after state_call_entity_init(), aka. during game
  if (entity_init_called && ent.init_f != NULL) { ent.init_f(&ent); }

  return id;
}
int state_entity_add_empty(vec3 pos, vec3 rot, vec3 scl)
{
  return state_entity_add(pos, rot, scl, -1, -1, 0, 0, NULL, NULL, NULL, NULL, NULL, -1);
}

// int state_entity_add_mesh(entity_t* e, int mesh, int mat)
// {
//   // continue here
//   //   - make inline state_add_mesh_id()
//   //   - test multiple meshes of devil_paladin
//   //   - test if debug draw still works
// 
//   ERR_CHECK(e->mesh_count +1 <= ENTITY_MESH_MAX, "cant add another mesh/material already at maximum of %d.\n", ENTITY_MESH_MAX);
// 
//   e->mesh[e->mesh_count] = mesh;
//   e->mat[e->mesh_count]  = mat;
//   
//   return e->mesh_count++;
// }

int state_entity_duplicate(entity_t* e, vec3 offset)
{
  vec3 pos;
  vec3_add(e->pos, offset, pos);
  int dupe = state_entity_add_from_template(pos, e->rot, e->scl, e->template_idx);

  // attached point light
  if (e->point_light_idx >= 0)
  {
    bool error = false;
    point_light_t* p = state_point_light_get(e->point_light_idx, &error); ASSERT(!error);
    state_point_light_add(p->offset, p->color, p->intensity, dupe);
  }

  return dupe;
  // return state_entity_add(pos, e->rot, e->scl, e->mesh, e->mat, e->init_f, e->update_f, e->table_idx);
}
void state_entity_remove_id(int id)
{
  ERR_CHECK(id >= 0 && id < world_arr_len, "removing invalid entity id");
  ERR_CHECK(!world_arr[id].is_dead, "removing already 'dead' entity");
 
  
  if (world_arr[id].cleanup_f != NULL)
  { world_arr[id].cleanup_f(&world_arr[id]); }

  if (world_arr[id].phys_flag != 0)
  { phys_remove_obj(id); }

  if (world_arr[id].point_light_idx >= 0)
  {
    state_point_light_remove(world_arr[id].point_light_idx);
    // world_arr[id].point_light_idx = -1;
  }

  if (world_arr[id].parent > -1)
  {
    state_entity_remove_child_id(world_arr[id].parent, id, true);
  }
  // for (int i = 0; i < world_arr[id].children_len; ++i)
  while (world_arr[id].children_len > 0)  
  {
    state_entity_remove_child_id(id, world_arr[id].children[0], true);
  }

  world_arr[id].is_dead = true;
  arrput(world_dead_arr, id);
  world_dead_arr_len++;
 
  if (core_data->outline_id == id) { core_data->outline_id = -1; }

  event_sys_trigger_entity_removed(id);
}
entity_t* state_entity_get_dbg(int id, bool* error, char* _file, int _line)
{
  // ERR_CHECK(id >= 0 && id < world_arr_len, "invalid entity id: %d, [file: %s, line: %d]", id, file, line);
  // ERR_CHECK(!world_arr[id].is_dead, "requested dead entity: %d, [file: %s, line: %d]", id, file, line);
  *error = id < 0 || id >= world_arr_len || world_arr[id].is_dead;
  entity_t* rtn = NULL;
  if (*error) 
  { P_ERR("state_entity_get() error\n  -> id: %d, world_arr_len: %d | is_dead: %s\n", id, world_arr_len, STR_BOOL(world_arr[id].is_dead)); }
  else { rtn = &world_arr[id]; }
  return rtn;
}

void state_entity_add_child(entity_t* p, entity_t* c, bool keep_transform)
{
  if (c->parent > -1) 
  { P_ERR("parenting child which is already parented.\n  -> parent: %d, child: %d, childs parent: %d\n", p->id, c->id, c->parent); return; } 

  // @TODO: @BUGG: keep_transform doesnt work
  // offset the child by the parents position to maintain its current global pos 
  if (keep_transform)
  {
    // vec3 offs; 
    // vec3_div(c->pos, p->scl, offs);
    // ENTITY_SET_POS(c, offs);

    // ENTITY_MOVE(c, VEC3_NEGATE(p->pos));
    
    // ENTITY_SET_POS(c, VEC3_DIV(c->pos, p->scl));
    // ENTITY_MOVE(c, VEC3_NEGATE(p->pos));
    

    // @TODO: rotation

    vec3_div(c->scl, p->scl, c->scl);
  }

  arrput(p->children, c->id);
  p->children_len++;
  c->parent = p->id;
  // PF("-> parent: %d, child: %d\n", p->id, c->id);
  
  event_sys_trigger_entity_parented(p->id, c->id);
}

void state_entity_remove_child(entity_t* p, entity_t* c, bool keep_transform)
{
  // offset the child by the parents position to maintain its current global pos 
  if (keep_transform)
  {
    // ENTITY_MOVE(c, p->pos);
    
    // vec3 offs; 
    // vec3_div(c->pos, p->scl, offs);
    // ENTITY_SET_POS(c, offs);
    
    // ENTITY_MOVE(c, VEC3_MUL(c->pos, p->scl));
    ENTITY_SET_POS(c, VEC3_MUL(c->pos, p->scl));
    
    // vec3_negate(p->pos, offs);
    // ENTITY_MOVE(c, offs);
    ENTITY_MOVE(c, p->pos);

    // // @TODO: rotation
    vec3_mul(c->scl, p->scl, c->scl);

  }

  for (int i = 0; i < p->children_len; ++i)
  {
    if (p->children[i] == c->id)
    {
      arrdel(p->children, i);
      p->children_len--;
      // PF("%d->child[%d]: %d removed\n", parent, i, child);
      break;
    }
  }

  c->parent = -1;
  // PF("in remove_child(): ");P_INT(c->parent);

  event_sys_trigger_entity_parent_removed(p->id, c->id);
}


void state_entity_get_total_children_len(entity_t* e, u32* len)
{
  // entity_t* e = state_entity_get(id);
  *len += e->children_len;
  for (u32 i = 0; i < e->children_len; ++i)
  {
    state_entity_get_total_children_len_id(e->children[i], len);
  }
}

// @NOTE: now inlined in state.h
// void state_entity_local_model(int id, mat4 out)
// void state_entity_local_model(entity_t* e, mat4 out)
// {  
//   if (id < 0 || id >= world_arr_len) 
//   { 
//     P_ERR("local model with invalid id. id'%d'", id); 
//     return;
//   }
//   
//   entity_t* e = state_entity_get(id);
//   mat4_make_model(e->pos, e->rot, e->scl, out);
// }
void state_entity_update_global_model_dbg(entity_t* e, char* _file, int _line)
{
  if (!e->is_moved) { return; }

  if (e->parent >= 0)
  {
    // apply parent model-matrix, parent_model * child_model = child_model
    // also track the transform delta this causes
    
    // vec3 pre_pos, post_pos, delta_pos;
    // mat4_get_pos(e->model, pre_pos);  // model from last frame
    
    mat4_make_model(e->pos, e->rot, e->scl, e->model);  // parent indipendent
    entity_t* p = state_entity_get(e->parent);
    mat4_mul(p->model, e->model, e->model);
    
    // mat4_get_pos(e->model, post_pos); // model after parent-transform
    // vec3_sub(post_pos, pre_pos, delta_pos); 
    // vec3_add(e->delta_pos, delta_pos, e->delta_pos); // @NOTE: acting fucky wucky
   
    // @UNSURE: fuck's this
    // if (core_data_is_play())
    // {
    //   // P_VEC3(pre_pos);
    //   // P_VEC3(post_pos);
    //   // P_VEC3(delta_pos);
    //   debug_draw_sphere_register(pre_pos, 0.2f,  RGB_F(1, 0, 0));
    //   debug_draw_sphere_register(post_pos, 0.2f, RGB_F(0, 1, 0));
    //   vec3 start, end;
    //   vec3_mul_f(delta_pos, 1.0f, end);
    //   debug_draw_line_register(start, end, RGB_F_RGB(0.5f));
    //   vec3_add(delta_pos, post_pos, start);
    //   vec3_add(end, post_pos, end);
    //   debug_draw_line_register(start, end, RGB_F_RGB(1.0f));
    // }
  }
  else
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, e->rot, e->scl, e->model);
  }
  for (int i = 0; i < e->children_len; ++i)
  {
    // @NOTE: setting 'is_moved' updates children next 'state_update()'
    //        just updating doesn't work
    // state_entity_update_global_model(e->children[i]);
    entity_t* c = state_entity_get(e->children[i]);
    c->is_moved = true;
  }
  // phys objs get reset in program.c program_sync_phys() 
  e->is_moved = e->phys_flag != 0 ? e->is_moved : false;
}
void state_entity_global_model_no_rotation(int id, mat4 out)
{
  if (id < 0 || id >= world_arr_len) 
  { 
    P_ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_entity_get(id);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), e->scl, out);
    entity_t* p = state_entity_get(e->parent);
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
  if (id < 0 || id >= world_arr_len) 
  { 
    P_ERR("no scale model with invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_entity_get(id);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    // mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    mat4_make_model(e->pos, e->rot, VEC3(1), out);
    entity_t* p = state_entity_get(e->parent);
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
  if (id < 0 || id >= world_arr_len) 
  { 
    P_ERR("no scale model with invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_entity_get(id);

  if (e->parent >= 0)
  {
    entity_t* p = state_entity_get(e->parent);
    // vec3 p_pos;
    // mat4_get_pos(p->model, p_pos);
    // vec3 pos;
    // mat4_get_pos(e->model, pos);
    // same as state_entity_local_model()
    // mat4_make_model(VEC3_ADD(p_pos, e->pos), VEC3(0), VEC3(1), out);
    mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
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
  if (id < 0 || id >= world_arr_len) 
  { 
    P_ERR("invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_entity_get(id);

  if (e->parent <= -1) { vec3_copy(e->scl, out); return; }
  
  state_entity_global_scale(e->parent, out);
  vec3_mul(out, e->scl, out);
}

// -- lights --

dir_light_t* state_dir_light_get_arr(int* len)
{
  *len = dir_lights_arr_len;
  return dir_lights_arr;
}

bool state_dir_light_add(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y)
{
  if (dir_lights_arr_len >= DIR_LIGHTS_MAX -1) { return false; }
  
  dir_light_t l;
  // vec3_copy(pos, l.pos);
  vec3_copy(dir, l.dir);
  vec3_copy(color, l.color);
  // vec3_copy(ambient, l.ambient);
  // vec3_copy(diffuse, l.diffuse);
  l.intensity    = intensity;
  l.cast_shadow  = cast_shadow;
  l.shadow_map_x = shadow_map_x;
  l.shadow_map_y = shadow_map_y;
  if (cast_shadow && dir_lights_arr_len == 0) // only light #0 can cast shadows
  { framebuffer_create_shadowmap(&l.fb_shadow.buffer, &l.fb_shadow.fbo, l.shadow_map_x, l.shadow_map_y); }
  
  dir_lights_arr[dir_lights_arr_len++] = l;
  return true;
}

void state_dir_light_remove(int idx)
{
  ERR_CHECK(idx >= 0 && idx < dir_lights_arr_len, "'idx' passed to 'state_dir_light_remove()' invalid: '%d', max: '%d'", idx, dir_lights_arr_len);
 
  // move all lights down one to replace the given light
  for (int i = idx; i < dir_lights_arr_len +1; ++i)
  {
    dir_lights_arr[i] = dir_lights_arr[i +1];
  }
  dir_lights_arr_len--;
}

// --- point lights ---

point_light_t* state_point_light_get_arr(int* len, int* dead_len)
{
  *len      = point_lights_arr_len;
  *dead_len = point_lights_dead_arr_len;
  return point_lights_arr;
}

point_light_t* state_point_light_get_dbg(int id, bool* error, const char* _file, const int _line)
{
  if (id < 0 || id >= point_lights_arr_len) 
  { ERR("id: %d, point_lights_arr_len: %d \ncalled from: \"%s\", line: %d\n", id, point_lights_arr_len, _file, _line); *error = true; return NULL; }
  *error = false;
  return &point_lights_arr[id];
}

int state_point_light_add_empty(vec3 pos, rgbf color, float intensity)
{
  int id = state_entity_add_empty(pos, VEC3(0), VEC3(1));
  return state_point_light_add(VEC3(0), color, intensity, id);
}


int state_point_light_add(vec3 offset, rgbf color, float intensity, int entity_id)
{
  if (point_lights_arr_len - point_lights_dead_arr_len >= POINT_LIGHTS_MAX -1) 
  { return -1; P_ERR("tried adding point light but already max amount of point lights"); }
  
  point_light_t l;
  l.entity_id = entity_id;
  l.is_dead = false;
  vec3_copy(offset, l.offset);
  vec3_copy(color, l.color);
  l.intensity    = intensity;
  
  // point_lights_arr[point_lights_arr_len++] = l;
  
  int id = -1;
  // check for free slot
  if (point_lights_dead_arr_len > 0)
  {
    id = arrpop(point_lights_dead_arr);
    l.id = id;
    point_lights_dead_arr_len--;
    point_lights_arr[id] = l;
  }
  else
  {
    id = point_lights_arr_len;
    l.id = id;
    arrput(point_lights_arr, l);
    point_lights_arr_len++;
  }
 
  entity_t* e = state_entity_get(entity_id);
  if (e->point_light_idx < 0)
  {
    e->point_light_idx = id;
    // l.entity_id = entity_id;
    // l.pos_ptr = e->pos;
    // P_VEC3(l.pos_ptr);
  }
  else { ERR("tried attaching point light to entity that already has one"); }
  
  PF(" -> added pointlight entity: e: %d, l: %d\n", entity_id, id);

  return id;
}

void state_point_light_remove(int id)
{
  ERR_CHECK(id >= 0 && id < point_lights_arr_len, "'id' passed to 'state_point_light_remove()' invalid: '%d', max: '%d'", id, point_lights_arr_len);
  ERR_CHECK(!point_lights_arr[id].is_dead, "removing already 'dead' point_light");

  // remove from attached entity
  if (point_lights_arr[id].entity_id >= 0)
  {
    entity_t* e = state_entity_get(point_lights_arr[id].entity_id);
    e->point_light_idx = -1;
  }
  
  point_lights_arr[id].is_dead = true;
  arrput(point_lights_dead_arr, id);
  point_lights_dead_arr_len++;
}

