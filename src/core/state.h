#ifndef STATE_H
#define STATE_H

#include "global/global.h"
#include "core/types/types.h"

// #define WORLD_MAX 12
#define DIR_LIGHTS_MAX   6    // depends on max number set in lighting shader
#define POINT_LIGHTS_MAX 12   // depends on max number set in lighting shader

// func decls --------------------------------------------------- 

void state_init();
void state_call_entity_init();  // calls init func ptr on all entities
void state_update(float dt);

void state_clear_scene();

entity_t* state_get_entity_arr(int* len, int* dead_len);
int state_add_entity_from_template(vec3 pos, vec3 rot, vec3 scl, int table_idx);
int state_add_entity(vec3 pos, vec3 rot, vec3 scl, int mesh, int mat, entity_phys_flag phys_flag, init_callback* init_f, update_callback* update_f, collision_callback* collision_f, trigger_callback* trigger_f, int table_idx);
int state_add_empty_entity(vec3 pos, vec3 rot, vec3 scl);
int state_duplicate_entity(int id, vec3 offset);

void state_remove_entity(int id);

entity_t* state_get_entity_dbg(int id, bool* error, char* file, int line);
#define state_get_entity(id, error) state_get_entity_dbg(id, error, __FILE__, __LINE__)

void state_entity_add_child(int parent, int child);
void state_entity_remove_child(int parent, int child);

void state_entity_local_model(int id, mat4 out);
void state_entity_update_global_model_dbg(int id, char* file, int line);
#define state_entity_update_global_model(id)  state_entity_update_global_model_dbg(id, __FILE__, __LINE__) 
void state_entity_global_model_no_rotation(int id, mat4 out);
void state_entity_model_no_scale(int id, mat4 out);
void state_entity_model_no_scale_rotation(int id, mat4 out);
void state_entity_global_scale(int id, vec3 out);


// @NOTE: make structure_t describing an entity and all its children
structure_t state_make_structure_from_entity(int id);
// @NOTE: needed internally for 'state_make_structure_from_entity()'
void state_structure_add_entity_recursive(structure_t* s, entity_t* e);

dir_light_t* state_get_dir_light_arr(int* len);
bool state_add_dir_light(vec3 pos, vec3 dir, rgbf color, float intensity, bool cast_shadow, int shadow_map_x, int shadow_map_y);
void state_remove_dir_light(int idx);


point_light_t* state_get_point_light_arr(int* len);
point_light_t* state_get_point_light(int id, bool* error);
// bool state_add_point_light(vec3 pos, rgbf color, float intensity);
int state_add_point_light_empty(vec3 pos, rgbf color, float intensity);           // point_light with new empty entity
int state_add_point_light(vec3 pos, rgbf color, float intensity, int entity_id);  // point_light atttached to existing entity
void state_remove_point_light(int idx);

#endif
