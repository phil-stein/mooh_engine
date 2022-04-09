#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "global/global.h"
#include "core/terrain.h"
#include "core/types/types.h"
#include "math/math_inc.h"


// used for shoving f32 into byte array
typedef union f32_wrapper_t
{
  u32 u_val;
  f32 f_val;
}f32_wrapper_t;


void serialization_test();


void serialization_write_scene_to_file(const char* name);
void serialization_load_scene_from_file(const char* name);

void serialization_serialize_scene(u8** buffer);
void serialization_serialize_entity(u8** buffer, entity_t* e);
void serialization_serialize_dir_light(u8** buffer, dir_light_t* l);
void serialization_serialize_point_light(u8** buffer, point_light_t* l);

void serialization_deserialize_scene(u8* buffer, u32* offset);
void serialization_deserialize_entity(u8* buffer, u32* offset);
void serialization_deserialize_dir_light(u8* buffer, u32* offset);
void serialization_deserialize_point_light(u8* buffer, u32* offset);


void serialization_write_terrain_to_file(const char* name);
void serialization_load_terrain_from_file(const char* name);

void serialization_serialize_terrain(u8** buffer);
void serialization_serialize_terrain_layout(u8** buffer, terrain_layout_t* l);

void serialization_deserialize_terrain(u8* buffer, u32* offset);
void serialization_deserialize_terrain_layout(u8* buffer, u32* offset, terrain_layout_t* l);

void serialization_serialize_u8(u8** buffer, u8 val);
void serialization_serialize_u32(u8** buffer, u32 val);
void serialization_serialize_s32(u8** buffer, int val);
void serialization_serialize_f32(u8** buffer, f32 val);
void serialization_serialize_vec2(u8** buffer, vec2 val);
void serialization_serialize_ivec2(u8** buffer, ivec2 val);
void serialization_serialize_vec3(u8** buffer, vec3 val);
void serialization_serialize_str(u8** buffer, char* val);

u8    serialization_deserialize_u8(u8* buffer, u32* offset);
u32   serialization_deserialize_u32(u8* buffer, u32* offset);
int   serialization_deserialize_s32(u8* buffer, u32* offset);
f32   serialization_deserialize_f32(u8* buffer, u32* offset);
void  serialization_deserialize_vec2(u8* buffer, u32* offset, vec2 out);
void  serialization_deserialize_ivec2(u8* buffer, u32* offset, ivec2 out);
void  serialization_deserialize_vec3(u8* buffer, u32* offset, vec3 out);
char* serialization_deserialize_str(u8* buffer, u32* offset);

#endif
