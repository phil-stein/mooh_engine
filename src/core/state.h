#ifndef STATE_H
#define STATE_H

#include "global/global.h"
#include "core/types/types.h"

// #define WORLD_MAX 12
#define DIR_LIGHTS_MAX   6    // depends on max number set in lighting shader
#define POINT_LIGHTS_MAX 12   // depends on max number set in lighting shader

// shared variable dont use this just for error detection in state_get_entity()
extern bool __state_get_entity_error_shared;

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
entity_t* state_get_entity_arr(int* len, int* dead_len);
// @DOC: get array[][] for each entity_template and the id's of the entites who belong to it
//       arr[ENTITY_TEMPLATE_X][Y]: the Y'th entity of template type ENTITY_TEMPLATE_X
//       len: gets set to the arr's length
int** state_get_template_entity_idxs_arr(int* len);

// @DOC: add an entity, based on an entity_template
//       returns added entities id
//       pos: position
//       rot: rotation
//       scl: scale
//       template_idx: idx for entity_template_get() in entity_template.c
int state_add_entity_from_template(vec3 pos, vec3 rot, vec3 scl, int template_idx);
// @DOC: add an entity
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
int state_add_entity(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, s64 tags_flag, entity_phys_flag phys_flag, init_callback* init_f, update_callback* update_f, collision_callback* collision_f, trigger_callback* trigger_f, int template_idx);
// @DOC: add an empty entity
//       returns added entities id
//       pos: position
//       rot: rotation
//       scl: scale
int state_add_empty_entity(vec3 pos, vec3 rot, vec3 scl);
// @DOC: duplicate existing entity
//       returns added entities id
//       id:     id of entity to be duplicated
//       offset: position offset given to new entity
int state_duplicate_entity(int id, vec3 offset);

// @DOC: remove an entity, aka. mark it dead
//       id: id of entity to be removed
void state_remove_entity(int id);

// @DOC: get pointer to entity via its id
//       id:    id of entity 
//       error: is set to true if id is invalid
entity_t* state_get_entity_dbg(int id, bool* error, char* _file, int _line);
#define state_get_entity_err(id, error) state_get_entity_dbg(id, error, __FILE__, __LINE__)
#define state_get_entity(id)            state_get_entity_dbg(id, &__state_get_entity_error_shared, __FILE__, __LINE__);   \
                                        ERR_CHECK(!__state_get_entity_error_shared, "get_entity failed\n") 

// @DOC: add a child to an entity
//       parent: id of entity to be the parent
//       child:  id of entity to be the child
void state_entity_add_child(int parent, int child);
// @DOC: remove a child from an entity
//       parent: id of parent entity 
//       child:  id of child entity 
void state_entity_remove_child(int parent, int child);
// @DOC: removes old parent if present and sets new parent
//       parent: new parent entity id
//       child:  new child entity id
void state_entity_add_child_remove_parent(int parent, int child);

// @DOC: entity model matrix without parent entities influence
//       id:  id of entity
//       out: get filled with model matrix
void state_entity_local_model(int id, mat4 out);
// @DOC: recalculate entities model matrix including parent incluence
//       id:  id of entity
void state_entity_update_global_model_dbg(int id, char* _file, int _line);
#define state_entity_update_global_model(id)  state_entity_update_global_model_dbg(id, __FILE__, __LINE__) 
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

// @NOTE: structures not working
// @DOC: make structure_t describing an entity and all its children
structure_t state_make_structure_from_entity(int id);
// @NOTE: structures not working
// @DOC: needed internally for 'state_make_structure_from_entity()'
void state_structure_add_entity_recursive(structure_t* s, entity_t* e);

// @DOC: get the array of all dir lights
//       len: gets set to arr's length
dir_light_t* state_get_dir_light_arr(int* len);
// @DOC: add a dir light
//       pos:          position, used for shadows
//       dir:          direction
//       color:        color
//       intensity:    intensity, mult for light calc
//       cast_shadow:  set true to cast shadows
//       shadow_map_x: shadow framebuffer resolution x
//       shadow_map_y: shadow framebuffer resolution y
bool state_add_dir_light(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y);
// @DOC: removes a dir light
//       idx: idx of light to be removed
void state_remove_dir_light(int idx);

// @DOC: get the array of all point lights
//       len:      gets set to arr's length
//       dead_len: gets set to how many point lights in that array are marked dead
point_light_t* state_get_point_light_arr(int* len, int* dead_len);
// @DOC: get pointer to point light via its id
//       id:    id of point light
//       error: is set to true if id is invalid
point_light_t* state_get_point_light_dbg(int id, bool* error, const char* _file, const int _line);
#define        state_get_point_light(id, error) state_get_point_light_dbg((id), (error), __FILE__, __LINE__) 
// @DOC: adds empty entity and a point light to it
//       pos:       pos for entity
//       color:     color for point light
//       intensity: intensity for point light
int state_add_point_light_empty(vec3 pos, rgbf color, float intensity);          
// @DOC: add a point light, couple to existing entity
//       offset:    offset to entities pos for point light
//       color:     color
//       intensity: intensity, mult for light calc
//       entity_id: id of entity to be attached to
int state_add_point_light(vec3 offset, rgbf color, float intensity, int entity_id); 
// @DOC: remove a point light, aka. mark it dead
//       id: id of point light to be removed
void state_remove_point_light(int id);

#endif
