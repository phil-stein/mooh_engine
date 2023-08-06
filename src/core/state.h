#ifndef CORE_STATE_H
#define CORE_STATE_H

#include "global/global.h"
#include "core/types/types.h"

// #define WORLD_MAX 12
#define DIR_LIGHTS_MAX   6    // depends on max number set in lighting shader
#define POINT_LIGHTS_MAX 12   // depends on max number set in lighting shader

// shared variable dont use this just for error detection in state_entity_get()
extern bool __state_entity_get_error_shared;
// shared variable used to allow inlining funcs that need world_arr_len
extern int* __state_world_arr_len_ptr_shared;

// @DOC: get two entities, meant for usage inside state_...() functions
//       id0, id1:     id's of the two entites
//       name0, name1: name given to the entity_t* var belonging to id0, id1
//       err:          line of code run on failure, i.e. 'ERR("failed")'
#define GET_ENTITY_ERR_CHECK_2(id0, id1, name0, name1, err)                       \
  if ((id0) < 0 || (id1) < 0 || (id0) >= (*__state_world_arr_len_ptr_shared) ||   \
      (id1) >= (*__state_world_arr_len_ptr_shared) || (id0) == (id1))             \
  { err; return; }                                                                \
  entity_t* (name0) = state_entity_get((id0));                                    \
  entity_t* (name1) = state_entity_get((id1));                                   


// func decls --------------------------------------------------- 

// @DOC: initialize, call this before any other calls to state_...()
void state_init();
// @DOC: calls entity_t->init_f on all entities
void state_call_entity_init();  // calls init func ptr on all entities
// @DOC: updates state logic, also calls entity_t->update_f, syncs point_light & entity pos, call once a frame
void state_update();

// @DOC: clear all entities, dir lights & point lights 
void state_clear_scene();

// @DOC: get the array of all entities
//       len:      gets set to returned arr's length
//       dead_len: gets set to how many entites in that array are marked dead
entity_t* state_entity_get_arr(int* len, int* dead_len);
// @DOC: get array[][] for each entity_template and the id's of the entites who belong to it
//       arr[ENTITY_TEMPLATE_X][Y]: the Y'th entity of template type ENTITY_TEMPLATE_X
//       len: gets set to the arr's length
int** state_entity_get_template_idxs_arr(int* len);

// @DOC: add an entity, based on an entity_template
//       also adds the appropriate phys objs
//       returns added entities id
//       pos: position
//       rot: rotation
//       scl: scale
//       template_idx: idx for entity_template_get() in entity_template.c
int state_entity_add_from_template(vec3 pos, vec3 rot, vec3 scl, int template_idx);
// @DOC: add an entity
//       ! doesnt add any phys objs, only state_add_entity_from template() does that
//       returns added entities id
//       pos:          position
//       rot:          rotation
//       scl:          scale
//       mesh:         assetm idx for a mesh
//       tags_flag:    flag with tags defined in data/entity_tags.h
//       mat:          assetm idx for a material
//       phys_flag:    physics 'components' to be attached
//       init_f:       initialization function called on start of game
//       update_f:     update function called every frame
//       collision_f:  called every time entity collides, only if entity has a collider
//       trigger_f:    called every time entity collides and entity or colliding object is trigger, only if entity has a collider
//       template_idx: entity_template idx, -1 for no template
int state_entity_add(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, s64 tags_flags, entity_phys_flag phys_flag, init_callback* init_f, update_callback* update_f, cleanup_callback* cleanup_f, collision_callback* collision_f, trigger_callback* trigger_f, int template_idx);
// @DOC: add an empty entity
//       returns added entities id
//       pos: position
//       rot: rotation
//       scl: scale
int state_entity_add_empty(vec3 pos, vec3 rot, vec3 scl);

// @DOC: add a mesh and material to an entities array
//       e:       entity to be modified
//       mesh:    assetm idx for a mesh
//       mat:     assetm idx for a material
//       returns: mat & mesh idx into entity array
int state_entity_add_mesh(entity_t* e, int mesh, int mat);

// @DOC: remove an entity, aka. mark it dead
//       id: id of entity to be removed
//       e:  entity to be removed
void state_entity_remove_id(int id);
INLINE void state_entity_remove(entity_t* e)
{ state_entity_remove_id(e->id); }

// @DOC: get pointer to entity via its id
//       id:    id of entity 
//       error: is set to true if id is invalid
entity_t* state_entity_get_dbg(int id, bool* error, char* _file, int _line);
#define state_entity_get_err(id, error) state_entity_get_dbg(id, error, __FILE__, __LINE__)
#define state_entity_get(id)            state_entity_get_dbg(id, &__state_entity_get_error_shared, __FILE__, __LINE__);   \
                                        ERR_CHECK(!__state_entity_get_error_shared, "get_entity [%d] failed\n", (id)) 

// @DOC: duplicate existing entity
//       returns added entities id
//       id:     id of entity to be duplicated
//       offset: position offset given to new entity
int state_entity_duplicate(entity_t* e, vec3 offset);
INLINE int state_entity_duplicate_id(int id, vec3 offset)
{
  entity_t* e = state_entity_get(id);
  return state_entity_duplicate(e, offset);
}

// @DOC: remove a child from an entity
//       p:      parent entity
//       parent: id of parent entity 
//       c:      child entity
//       child:  id of child entity 
void state_entity_remove_child(entity_t* p, entity_t* c, bool keep_transform);
INLINE void state_entity_remove_child_id(int parent, int child, bool keep_transform)
{
  bool err = false;
  entity_t* p = state_entity_get_err(parent, &err);
  entity_t* c = state_entity_get_err(child,  &err);
  if (err) { P_ERR("un-parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); return; }
  state_entity_remove_child(p, c, keep_transform);
}

// @DOC: add a child to an entity
//       p:              entity to be the parent
//       parent:         id of entity to be the parent
//       c:              entity to be the child
//       child:          id of entity to be the child
//       keep_transform: offset the child by the parents transform 
//                       to maintain global pos 
void state_entity_add_child(entity_t* p, entity_t* c, bool keep_transform);
INLINE void state_entity_add_child_id(int parent, int child, bool keep_transform)
{
  bool err = false;
  entity_t* p = state_entity_get_err(parent, &err);
  entity_t* c = state_entity_get_err(child,  &err);
  if (err) { P_ERR("un-parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); return; }
  state_entity_add_child(p, c, keep_transform);
}

// @DOC: removes old parent if present and sets new parent
//       p:      entity to be parent
//       parent: new parent entity id
//       c:      entity to be child
//       child:  new child entity id
INLINE void state_entity_add_child_remove_parent(entity_t* p, entity_t* c, bool keep_transform)
{
  if (c->parent >= 0 && c->parent != p->id)
  { state_entity_remove_child(p, c, true); }
  state_entity_add_child(p, c, keep_transform);
}
INLINE void state_entity_add_child_remove_parent_id(int parent, int child, bool keep_transform)
{
  bool err = false;
  entity_t* p = state_entity_get_err(parent, &err);
  entity_t* c = state_entity_get_err(child,  &err);
  if (err) { P_ERR("un-parenting invalid entity indices. parent'%d' <-> child'%d'", parent, child); return; }
  state_entity_add_child_remove_parent(p, c, keep_transform);
}

// @DOC: recursively count all childrens children_len
//      e:   entity
//      id:  id of entity
//      len: need to be 0
void state_entity_get_total_children_len(entity_t* e, u32* len);
INLINE void state_entity_get_total_children_len_id(int id, u32* len)
{
  entity_t* e = state_entity_get(id);
  state_entity_get_total_children_len(e, len);
}

// @DOC: entity model matrix without parent entities influence
//       id:  id of entity
//       out: get filled with model matrix
INLINE void state_entity_local_model(entity_t* e, mat4 out)
{
  mat4_make_model(e->pos, e->rot, e->scl, out);
}
INLINE void state_entity_local_model_id(int id, mat4 out)
{
  entity_t* e = state_entity_get(id);
  state_entity_local_model(e, out);
}
// @DOC: recalculate entities model matrix including parent incluence
//       id:  id of entity
// void state_entity_update_global_model_dbg(int id, char* _file, int _line);
void state_entity_update_global_model_dbg(entity_t* e, char* _file, int _line);
#define state_entity_update_global_model(e)  state_entity_update_global_model_dbg(e, __FILE__, __LINE__) 
INLINE void state_entity_update_global_model_id(int id)
{
  entity_t* e = state_entity_get(id);
  state_entity_update_global_model(e);
}

// @DOC: entity model matrix with parent entities influence, but no rotations
//       id:  id of entity
//       out: get filled with model matrix
void state_entity_global_model_no_rotation(int id, mat4 out);
// @DOC: @TODO:
void state_entity_model_no_scale(int id, mat4 out);
// @DOC: @TODO:
void state_entity_model_no_scale_rotation(int id, mat4 out);
// @DOC: @TODO:
void state_entity_global_scale(int id, vec3 out);

// @DOC: get the array of all dir lights
//       len: gets set to arr's length
dir_light_t* state_dir_light_get_arr(int* len);
// @DOC: add a dir light
//       pos:          position, used for shadows
//       dir:          direction
//       color:        color
//       intensity:    intensity, mult for light calc
//       cast_shadow:  set true to cast shadows
//       shadow_map_x: shadow framebuffer resolution x
//       shadow_map_y: shadow framebuffer resolution y
bool state_dir_light_add(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y);
// @DOC: removes a dir light
//       idx: idx of light to be removed
void state_dir_light_remove(int idx);

// @DOC: get the array of all point lights
//       len:      gets set to arr's length
//       dead_len: gets set to how many point lights in that array are marked dead
point_light_t* state_point_light_get_arr(int* len, int* dead_len);
// @DOC: get pointer to point light via its id
//       id:    id of point light
//       error: is set to true if id is invalid
point_light_t* state_point_light_get_dbg(int id, bool* error, const char* _file, const int _line);
#define        state_point_light_get(id, error) state_point_light_get_dbg((id), (error), __FILE__, __LINE__) 
// @DOC: adds empty entity and a point light to it
//       pos:       pos for entity
//       color:     color for point light
//       intensity: intensity for point light
int state_point_light_add_empty(vec3 pos, rgbf color, float intensity);          
// @DOC: add a point light, couple to existing entity
//       offset:    offset to entities pos for point light
//       color:     color
//       intensity: intensity, mult for light calc
//       entity_id: id of entity to be attached to
int state_point_light_add(vec3 offset, rgbf color, float intensity, int entity_id); 
// @DOC: remove a point light, aka. mark it dead
//       id: id of point light to be removed
void state_point_light_remove(int id);

#endif
