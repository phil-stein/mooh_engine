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
#ifdef EDITOR
bool entity_init_called = false;
#endif

static core_data_t* core_data = NULL;

// shared variable dont use this just for error detection in state_get_entity(), extern def in state.h
bool __state_get_entity_error_shared = false;

void state_init()
{
  core_data = core_data_get();

  int templates_len = 0;
  entity_template_get_all(&templates_len);
  MALLOC(template_entity_idxs_arr, templates_len * sizeof(int*));
  template_entity_idxs_arr_len = templates_len;
  for (u32 i = 0; i < template_entity_idxs_arr_len; ++i)
  { template_entity_idxs_arr[i] = NULL; }
}
void state_call_entity_init()
{
  for (int i = 0; i < world_arr_len; ++i)
  {
    if (world_arr[i].init_f != NULL)
    { world_arr[i].init_f(&world_arr[i]); }
  }
}

void state_update()
{
  // @NOTE: gets called in state_init(), this only affects the editor
#ifdef EDITOR
  if (!entity_init_called && core_data->scripts_act)
  {
    state_call_entity_init();
    entity_init_called = true;
  }
#endif

  for (int i = 0; i < world_arr_len; ++i)
  {
    if (world_arr[i].is_dead) { continue; }

    state_entity_update_global_model(i);
    
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

entity_t* state_get_entity_arr(int* len, int* dead_len)
{
  *len = world_arr_len;
  *dead_len = world_dead_arr_len;
  return world_arr;
}
int** state_get_template_entity_idxs_arr(int* len)
{
  *len = template_entity_idxs_arr_len;
  return template_entity_idxs_arr;
}

int state_add_entity_from_template(vec3 pos, vec3 rot, vec3 scl, int template_idx)
{
  const entity_template_t* def = entity_template_get(template_idx);
  int mesh = -1;
  if (!(strlen(def->mesh) == 1 && def->mesh[0] == '-')) // isnt equal to "-", that means no mesh
  { mesh = assetm_get_mesh_idx(def->mesh); }
  int mat  = 0; 
  if (def->mat > -1)    // isnt -1 as thats no mat
  { mat = assetm_get_material_idx(def->mat); }

  int id = state_add_entity(pos, rot, scl, mesh, mat, def->tags_flag, def->phys_flag, def->init_f, def->update_f, def->collision_f, def->trigger_f, template_idx);

  if (HAS_FLAG(def->phys_flag, ENTITY_HAS_BOX) && HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
  {
    vec3 half_extents;
    vec3 aabb[2];
    vec3_mul_f((f32*)def->aabb_size, 1, half_extents);
    vec3_copy(half_extents, aabb[0]);
    vec3_copy(half_extents, aabb[1]);
    vec3_mul_f(aabb[0], -1, aabb[0]);
    
    phys_add_obj_rb_box(id, pos, scl, def->mass, def->friction, aabb, (f32*)def->aabb_offset, def->is_trigger);
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
    
    phys_add_obj_box(id, pos, scl, aabb, (f32*)def->aabb_offset, def->is_trigger);
  }
  
  return id; 
}

int state_add_entity(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, s64 tags_flags, entity_phys_flag phys_flag, init_callback* init_f, update_callback* update_f, collision_callback* collision_f, trigger_callback* trigger_f, int template_idx)
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
  ent.mesh            = mesh;
  ent.mat             = mat;
  ent.point_light_idx = -1;
  ent.phys_flag       = phys_flag;
  ent.is_grounded     = false; 
  ent.init_f          = init_f;
  ent.update_f        = update_f;
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

  return id;
}
int state_add_empty_entity(vec3 pos, vec3 rot, vec3 scl)
{
  return state_add_entity(pos, rot, scl, -1, -1, 0, 0, NULL, NULL, NULL, NULL, -1);
}

int state_duplicate_entity(int id, vec3 offset)
{
  entity_t* e = state_get_entity(id);
  vec3 pos;
  vec3_add(e->pos, offset, pos);
  int dupe = state_add_entity_from_template(pos, e->rot, e->scl, e->template_idx);

  // attached point light
  if (e->point_light_idx >= 0)
  {
    bool error = false;
    point_light_t* p = state_get_point_light(e->point_light_idx, &error); ASSERT(!error);
    state_add_point_light(p->offset, p->color, p->intensity, dupe);
  }

  return dupe;
  // return state_add_entity(pos, e->rot, e->scl, e->mesh, e->mat, e->init_f, e->update_f, e->table_idx);
}
void state_remove_entity(int id)
{
  ERR_CHECK(id >= 0 && id < world_arr_len, "removing invalid entity id");
  ERR_CHECK(!world_arr[id].is_dead, "removing already 'dead' entity");
 
  if (world_arr[id].phys_flag != 0)
  { phys_remove_obj(id); }

  if (world_arr[id].point_light_idx >= 0)
  {
    state_remove_point_light(world_arr[id].point_light_idx);
    // world_arr[id].point_light_idx = -1;
  }

  if (world_arr[id].parent > -1)
  {
    state_entity_remove_child(world_arr[id].parent, id);
  }
  for (int i = 0; i < world_arr[id].children_len; ++i)
  {
    state_entity_remove_child(world_arr[id].id, world_arr[id].children[i]);
  }

  world_arr[id].is_dead = true;
  arrput(world_dead_arr, id);
  world_dead_arr_len++;
 
  if (core_data->outline_id == id) { core_data->outline_id = -1; }

  event_sys_trigger_entity_removed(id);
}
entity_t* state_get_entity_dbg(int id, bool* error, char* _file, int _line)
{
  // ERR_CHECK(id >= 0 && id < world_arr_len, "invalid entity id: %d, [file: %s, line: %d]", id, file, line);
  // ERR_CHECK(!world_arr[id].is_dead, "requested dead entity: %d, [file: %s, line: %d]", id, file, line);
  *error = id < 0 || id >= world_arr_len || world_arr[id].is_dead;
  if (*error) { PF("[ERROR] state_get_entity error\n  -> id: %d, world_arr_len: %d | is_dead: %s\n", id, world_arr_len, STR_BOOL(world_arr[id].is_dead)); }
  return &world_arr[id];
}

void state_entity_add_child(int parent, int child)
{
  if (parent < 0 || child < 0 || parent >= world_arr_len || child >= world_arr_len || child == parent) 
  {
    P_ERR("parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); 
    return;
  }
  
  entity_t* p = state_get_entity(parent);
  entity_t* c = state_get_entity(child);

  if (c->parent > -1) 
  { P_ERR("parenting child which is already parented."); return; } 

  // @TODO: doesnt work, maybe gets overwritten or not updated
  // // set offset in phys
  // if (HAS_FLAG(c->phys_flag, ENTITY_HAS_BOX))
  // {
  //   u32 phys_objs_len = 0;
  //   phys_obj_t* phys_objs = phys_get_obj_arr(&phys_objs_len);
  //   for (u32 i = 0; i < phys_objs_len; ++i)
  //   {
  //     phys_obj_t* obj = &phys_objs[i];
  //     if (obj->entity_idx == c->id)
  //     {
  //       P("peepee");
  //       vec3_add(obj->pos, c->pos, obj->pos);
  //       vec3_mul(obj->scl, c->scl, obj->scl);
  //     }
  //   }
  // }

  // offset the child by the parents position to maintain its current global pos
  vec3 offs; vec3_negate(p->pos, offs);
  ENTITY_MOVE(c, offs);

  arrput(p->children, child);
  p->children_len++;
  c->parent = parent;
  // PF("-> parent: %d, child: %d\n", p->id, c->id);
  
  event_sys_trigger_entity_parented(parent, child);
}
void state_entity_remove_child(int parent, int child)
{
  if (parent < 0 || child < 0 || parent >= world_arr_len || child >= world_arr_len || child == parent) 
  { 
    P_ERR("un-parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); 
    return;
  }
  
  entity_t* p = state_get_entity(parent);
  entity_t* c = state_get_entity(child);

  for (int i = 0; i < p->children_len; ++i)
  {
    if (p->children[i] == child)
    {
      arrdel(p->children, i);
      p->children_len--;
    }
  }
  c->parent = -1;

  event_sys_trigger_entity_parent_removed(parent, child);
}

void state_entity_add_child_remove_parent(int parent, int child)
{
  entity_t* c = state_get_entity(child);
  if (c->parent >= 0 && c->parent != parent)
  { state_entity_remove_child(parent, child); }
  state_entity_add_child(parent, child);
}

void state_entity_local_model(int id, mat4 out)
{  
  if (id < 0 || id >= world_arr_len) 
  { 
    P_ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_get_entity(id);
  mat4_make_model(e->pos, e->rot, e->scl, out);
}
void state_entity_update_global_model_dbg(int id, char* _file, int _line)
{
  // if (id == 0 || id == 1) { P("brrrrrr"); }
  if (id < 0 || id >= world_arr_len) 
  { 
    ERR("local model with invalid id. id'%d'", id); 
    return;
  }
  
  entity_t* e = state_get_entity(id); // &error); ERR_CHECK(!error, "state_get_entity() failed called from: '%s' line: %d\n", file, line);
  
  if (!e->is_moved) { return; }

  if (e->parent >= 0)
  {
    // apply parent model-matrix, parent_model * child_model = child_model
    // also track the transform delta this causes
    vec3 pre_pos, post_pos, delta_pos;
    mat4_get_pos(e->model, pre_pos);  // model from last frame
    
    mat4_make_model(e->pos, e->rot, e->scl, e->model);  // parent indipendent
    entity_t* p = state_get_entity(e->parent);
    mat4_mul(p->model, e->model, e->model);
    
    mat4_get_pos(e->model, post_pos); // model after parent-transform
    vec3_sub(post_pos, pre_pos, delta_pos); 
    // vec3_add(e->delta_pos, delta_pos, e->delta_pos); // @NOTE: acting fucky wucky
    
    if (core_data_is_play())
    {
      // P_VEC3(pre_pos);
      // P_VEC3(post_pos);
      // P_VEC3(delta_pos);
      debug_draw_sphere_register(pre_pos, 0.2f,  RGB_F(1, 0, 0));
      debug_draw_sphere_register(post_pos, 0.2f, RGB_F(0, 1, 0));
      vec3 start, end;
      vec3_mul_f(delta_pos, 1.0f, end);
      debug_draw_line_register(start, end, RGB_F_RGB(0.5f));
      vec3_add(delta_pos, post_pos, start);
      vec3_add(end, post_pos, end);
      debug_draw_line_register(start, end, RGB_F_RGB(1.0f));
    }
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
    entity_t* c = state_get_entity(e->children[i]);
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
  
  entity_t* e = state_get_entity(id);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), e->scl, out);
    entity_t* p = state_get_entity(e->parent);
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
  
  entity_t* e = state_get_entity(id);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    // mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    mat4_make_model(e->pos, e->rot, VEC3(1), out);
    entity_t* p = state_get_entity(e->parent);
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
  
  entity_t* e = state_get_entity(id);

  if (e->parent >= 0)
  {
    // same as state_entity_local_model()
    mat4_make_model(e->pos, VEC3(0), VEC3(1), out);
    entity_t* p = state_get_entity(e->parent);
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
  
  entity_t* e = state_get_entity(id);

  if (e->parent <= -1) { vec3_copy(e->scl, out); return; }
  
  state_entity_global_scale(e->parent, out);
  vec3_mul(out, e->scl, out);
}

// -- structures --

structure_t state_make_structure_from_entity(int id)
{
  entity_t* e = state_get_entity(id);
  
  structure_t s = STRUCTURE_T_INIT();
  state_structure_add_entity_recursive(&s, e);
  
  return s;
}
void state_structure_add_entity_recursive(structure_t* s, entity_t* e)
{
  if (e->parent >= 0) 
  { 
    arrput(s->entities, e->parent); 

    entity_t* p = state_get_entity(e->parent);

    state_structure_add_entity_recursive(s, p);
  }
}

// -- lights --

dir_light_t* state_get_dir_light_arr(int* len)
{
  *len = dir_lights_arr_len;
  return dir_lights_arr;
}

bool state_add_dir_light(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y)
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

void state_remove_dir_light(int idx)
{
  ERR_CHECK(idx >= 0 && idx < dir_lights_arr_len, "'idx' passed to 'state_remove_dir_light()' invalid: '%d', max: '%d'", idx, dir_lights_arr_len);
 
  // move all lights down one to replace the given light
  for (int i = idx; i < dir_lights_arr_len +1; ++i)
  {
    dir_lights_arr[i] = dir_lights_arr[i +1];
  }
  dir_lights_arr_len--;
}

// --- point lights ---

point_light_t* state_get_point_light_arr(int* len, int* dead_len)
{
  *len      = point_lights_arr_len;
  *dead_len = point_lights_dead_arr_len;
  return point_lights_arr;
}

point_light_t* state_get_point_light_dbg(int id, bool* error, const char* _file, const int _line)
{
  if (id < 0 || id >= point_lights_arr_len) 
  { ERR("id: %d, point_lights_arr_len: %d \ncalled from: \"%s\", line: %d\n", id, point_lights_arr_len, _file, _line); *error = true; return NULL; }
  *error = false;
  return &point_lights_arr[id];
}

int state_add_point_light_empty(vec3 pos, rgbf color, float intensity)
{
  int id = state_add_empty_entity(pos, VEC3(0), VEC3(1));
  return state_add_point_light(VEC3(0), color, intensity, id);
}


int state_add_point_light(vec3 offset, rgbf color, float intensity, int entity_id)
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
 
  entity_t* e = state_get_entity(entity_id);
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

void state_remove_point_light(int id)
{
  ERR_CHECK(id >= 0 && id < point_lights_arr_len, "'id' passed to 'state_remove_point_light()' invalid: '%d', max: '%d'", id, point_lights_arr_len);
  ERR_CHECK(!point_lights_arr[id].is_dead, "removing already 'dead' point_light");

  // remove from attached entity
  if (point_lights_arr[id].entity_id >= 0)
  {
    entity_t* e = state_get_entity(point_lights_arr[id].entity_id);
    e->point_light_idx = -1;
  }
  
  point_lights_arr[id].is_dead = true;
  arrput(point_lights_dead_arr, id);
  point_lights_dead_arr_len++;
}

