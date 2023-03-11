#ifndef SAVE_SYS_H
#define SAVE_SYS_H

#include "global/global.h"
#include "core/terrain.h"
#include "core/types/types.h"
#include "math/math_inc.h"


// @NOTE: 0x010 = 1.0, 0x001 = 0.1, etc.
//        serialized as u32
#define SAVE_SYS_VERSION 2

// @NOTE: mute or show serialization messages
// #define SERIALIZATION_P(txt) P(txt)
// #define SERALIZATION_PF(...) PF(__VA_ARGS__)
#define SERIALIZATION_P(txt)
#define SERALIZATION_PF(...) 


// @DOC: initialize, call this before any other calls to save_sys
void save_sys_init();


void save_sys_write_scene_to_file(const char* name);
void save_sys_load_scene_from_file(const char* name);

#ifdef EDITOR
void save_sys_write_scene_to_state_buffer();
void save_sys_load_scene_from_state_buffer();

void save_sys_write_empty_scene_to_file();
#endif

void save_sys_serialize_scene(u8** buffer);
void save_sys_serialize_entity(u8** buffer, entity_t* e);
void save_sys_serialize_dir_light(u8** buffer, dir_light_t* l);
void save_sys_serialize_point_light(u8** buffer, point_light_t* l);

void save_sys_deserialize_scene(u8* buffer, u32* offset);
void save_sys_deserialize_entity(u8* buffer, u32* offset);
void save_sys_deserialize_dir_light(u8* buffer, u32* offset);
int save_sys_deserialize_point_light(u8* buffer, u32* offset, int entity_id);


void save_sys_write_terrain_to_file(const char* name);
void save_sys_load_terrain_from_file(const char* name);

void save_sys_serialize_terrain(u8** buffer);
void save_sys_serialize_terrain_layout(u8** buffer, terrain_layout_t* l);

void save_sys_deserialize_terrain(u8* buffer, u32* offset);
void save_sys_deserialize_terrain_layout(u8* buffer, u32* offset, terrain_layout_t* l);

#endif
