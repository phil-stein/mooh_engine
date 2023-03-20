#ifndef SAVE_SYS_H
#define SAVE_SYS_H

#include "global/global.h"
#include "core/terrain.h"
#include "core/types/types.h"
#include "math/math_inc.h"


// @NOTE: 0x010 = 1.0, 0x001 = 0.1, etc.
//        serialized as u32
#define SAVE_SYS_VERSION 2

// @DOC: name given to file made by save_sys_write_empty_scene_to_file()
#define SAVE_SYS_EMPTY_SCENE_NAME "-_-_new_-_-"

// @NOTE: mute or show serialization messages
// #define SERIALIZATION_P(txt) P(txt)
// #define SERALIZATION_PF(...) PF(__VA_ARGS__)
#define SERIALIZATION_P(txt)
#define SERALIZATION_PF(...) 


// @DOC: initialize, call this before any other calls to save_sys
void save_sys_init();

// @DOC: serialize whole scene and write to a .scene file
//       name: name of file to be written to
void save_sys_write_scene_to_file(const char* name);
// @DOC: deserialize and load a scene from a .scene file
//       name: name of .scene file to be loaded
void save_sys_load_scene_from_file_dbg(const char* name, const char* _file, const int _line);
#define save_sys_load_scene_from_file(name) save_sys_load_scene_from_file_dbg((name), __FILE__, __LINE__)

// #ifdef EDITOR
// @DOC: serialize whole scene and write a buffer in save_sys.c 
void save_sys_write_scene_to_state_buffer();
// @DOC: deserialize and load a scene from a buffer in save_sys.c
void save_sys_load_scene_from_state_buffer();

// @DOC: writes an empty scene to file, name define by SAVE_SYS_EMPTY_SCENE_NAME
void save_sys_write_empty_scene_to_file();
// #endif // EDITOR

// @DOC: serialize the whole scene into byte buffer
//       buffer: pointer to stb_ds u8 array
void save_sys_serialize_scene(u8** buffer);
// @DOC: serialize an entity into byte buffer
//       buffer: pointer to stb_ds u8 array
//       e:      entity to be serialized
void save_sys_serialize_entity(u8** buffer, entity_t* e);
// @DOC: serialize a dir light into byte buffer
//       buffer: pointer to stb_ds u8 array
//       l:      dir light to be serialized
void save_sys_serialize_dir_light(u8** buffer, dir_light_t* l);
// @DOC: serialize a point light into byte buffer
//       buffer: pointer to stb_ds u8 array
//       l:      point light to be serialized
void save_sys_serialize_point_light(u8** buffer, point_light_t* l);

// @DOC: deserialize the whole scene from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer 
void save_sys_deserialize_scene(u8* buffer, u32* offset);
// @DOC: deserialize an entity from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer 
void save_sys_deserialize_entity(u8* buffer, u32* offset);
// @DOC: deserialize a dir light from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer 
void save_sys_deserialize_dir_light(u8* buffer, u32* offset);
// @DOC: deserialize a point light from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer
//       return point light idx
int save_sys_deserialize_point_light(u8* buffer, u32* offset, int entity_id);


// --- terrain ---

// @DOC: serialize and write the terrain to file
//       name: name of the file to be written to
void save_sys_write_terrain_to_file(const char* name);
// @DOC: load terrain from a file
//       name: name of file to be loaded
void save_sys_load_terrain_from_file(const char* name);

// @DOC: serialize terrain into byte buffer
//       buffer: pointer to stb_ds u8 array
void save_sys_serialize_terrain(u8** buffer);
// @DOC: serialize terrain layout into byte buffer
//       buffer: pointer to stb_ds u8 array
//       l:      pointer to terrain layout
void save_sys_serialize_terrain_layout(u8** buffer, terrain_layout_t* l);

// @DOC: deserialize terrain from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer 
void save_sys_deserialize_terrain(u8* buffer, u32* offset);
// @DOC: deserialize terrain layout from byte buffer
//       buffer: stb_ds u8 array with the data
//       offset: current offset into buffer 
//       l:      terrain_layout_t to be filled with loaded data
void save_sys_deserialize_terrain_layout(u8* buffer, u32* offset, terrain_layout_t* l);

#endif
