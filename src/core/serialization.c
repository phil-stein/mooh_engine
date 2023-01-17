#include "core/serialization.h"
#include "core/core_data.h"
#include "core/state.h"
#include "core/file_io.h"

#ifdef EDITOR
#include "core/camera.h"
#endif

#include "stb/stb_ds.h"


#define STR_BUF_MAX 256
char str_buf[STR_BUF_MAX] = "";

#define CUR_SCENE_NAME_MAX 128
char cur_scene_name[CUR_SCENE_NAME_MAX] = "";

#ifdef EDITOR
u8* state_buffer = NULL;
char state_buffer_scene_name[CUR_SCENE_NAME_MAX] = "";

vec3 state_cam_pos         = { 0, 0, 0 };
vec3 state_cam_orientation = { 0, 0, 0 };
#endif

static core_data_t* core_data = NULL;

void serialization_test()
{
  u8* buffer = NULL;
 
  u8  b0 = 128;
  serialization_serialize_u8(&buffer, b0);
  u32 u0 = 1234;
  serialization_serialize_u32(&buffer, u0);
  int s0 = -1234;
  serialization_serialize_s32(&buffer, s0);
  f32 f0 = 123.45678f;
  serialization_serialize_f32(&buffer, f0);
  vec2 v2_0 = { 123.45f, 678.9f };
  serialization_serialize_vec2(&buffer, v2_0);
  vec3 v3_0 = { 123.45f, 678.9f, 1234567.89f };
  serialization_serialize_vec3(&buffer, v3_0);
  char str0[] = "hello, world! this is a test :)";
  serialization_serialize_str(&buffer, str0);

  P_INT((int)arrlen(buffer));

  u32 offset = 0;

  u8 b0d = serialization_deserialize_u8(buffer, &offset);
  P_INT(b0d); ASSERT(b0d == b0);
  u32 u0d = serialization_deserialize_u32(buffer, &offset);
  P_U32(u0d); ASSERT(u0d == u0);
  int s0d = serialization_deserialize_s32(buffer, &offset);
  P_INT(s0d); ASSERT(s0d == s0);
  f32 f0d = serialization_deserialize_f32(buffer, &offset);
  P_F32(f0d); ASSERT(f0d == f0);
  vec2 v2_0d; serialization_deserialize_vec2(buffer, &offset, v2_0d);
  P_VEC2(v2_0d); ASSERT(vec2_equal(v2_0d, v2_0)); 
  vec3 v3_0d; serialization_deserialize_vec3(buffer, &offset, v3_0d);
  P_VEC3(v3_0d); ASSERT(vec2_equal(v3_0d, v3_0)); 
  serialization_deserialize_str(buffer, &offset);
  P_STR(str_buf);

  arrfree(buffer);

}


// ---- scene ----

void serialization_write_scene_to_file(const char* name)
{
  u8* buffer = NULL;

  // @UNSURE: add serialization version ???
  serialization_serialize_scene(&buffer);
  
  char path[128];
  sprintf(path, ASSET_PATH"%s", name);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));


  arrfree(buffer);
}

void serialization_load_scene_from_file(const char* name)
{
  u32 offset = 0;
  int length = 0;
  
  char path[128];
  sprintf(path, ASSET_PATH"%s", name);
  u8* buffer = (u8*)file_read_bytes(path, &length);
  
  serialization_deserialize_scene(buffer, &offset);

  ASSERT(strlen(name) < CUR_SCENE_NAME_MAX);
  strcpy(cur_scene_name, name);

  free(buffer);
}

#ifdef EDITOR
void serialization_write_scene_to_state_buffer()
{
  u8* buffer = NULL;

  serialization_serialize_scene(&buffer);
 
  // @TODO: 
  // !!! DO THIS NEXT
  // also add searching .h files for func-defs to term_docs

  state_buffer = realloc(state_buffer, arrlen(buffer) * sizeof(u8));
  memcpy(state_buffer, buffer, arrlen(buffer) * sizeof(u8));

  strcpy(state_buffer_scene_name, cur_scene_name);

  arrfree(buffer);

  camera_get_pos(state_cam_pos);
  camera_get_front(state_cam_orientation);
}

void serialization_load_scene_from_state_buffer()
{
  u32 offset = 0;
  
  serialization_deserialize_scene(state_buffer, &offset);

  strcpy(cur_scene_name, state_buffer_scene_name);

  camera_set_pos(state_cam_pos);
  camera_set_front(state_cam_orientation);
}

void serialization_write_empty_scene_to_file()
{
  u8* buffer = NULL;

  // serialization_serialize_scene(&buffer);
  serialization_serialize_u32(&buffer, 0); // dir_lights_len
  serialization_serialize_u32(&buffer, 0); // point_lights_len
  serialization_serialize_u32(&buffer, 0); // world_len
  
  SERIALIZATION_P("[serialization] serialized empty scene");

  char path[128];
  sprintf(path, ASSET_PATH"%s", "-_-_new_-_-");
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}
#endif

void serialization_serialize_scene(u8** buffer)
{
  // -- version --

  serialization_serialize_u32(buffer, SERIALIZATION_VERSION);
  
  // -- entities --

  int world_len = 0;
  int world_dead_len = 0;
  entity_t* world = state_get_entity_arr(&world_len, &world_dead_len);

  serialization_serialize_u32(buffer, world_len - world_dead_len);

  for (u32 i = 0; i < world_len; ++i)
  {
    if (world[i].is_dead) { continue; }
    serialization_serialize_entity(buffer, &world[i]);
  }

  // -- dir lights --
  
  int dir_lights_len = 0;
  dir_light_t* dir_lights = state_get_dir_light_arr(&dir_lights_len);
   
  serialization_serialize_u32(buffer, dir_lights_len);

  for (u32 i = 0; i < dir_lights_len; ++i)
  {
    serialization_serialize_dir_light(buffer, &dir_lights[i]);
  } 
   
  // -- point lights --

  int point_lights_len = 0;
  point_light_t* point_lights = state_get_point_light_arr(&point_lights_len);
   
  serialization_serialize_u32(buffer, point_lights_len);

  for (u32 i = 0; i < point_lights_len; ++i)
  {
    serialization_serialize_point_light(buffer, &point_lights[i]);
  } 
  
  SERIALIZATION_P("[serialization] serialized scene");
}
void serialization_deserialize_scene(u8* buffer, u32* offset)
{

  // clear pre-existing scene
  state_clear_scene();
 
  // -- version -- 
  
  u32 version = serialization_deserialize_u32(buffer, offset);

  // -- entities --

  u32 world_len = serialization_deserialize_u32(buffer, offset);
  P_U32(world_len);

  for (u32 i = 0; i < world_len; ++i)
  {
    serialization_deserialize_entity(buffer, offset);
  }
  
  // -- dir lights --
  
  u32 dir_lights_len = serialization_deserialize_u32(buffer, offset);
  P_U32(dir_lights_len);

  for (u32 i = 0; i < dir_lights_len; ++i)
  {
    serialization_deserialize_dir_light(buffer, offset);
  }

  // -- point lights --
   
  u32 point_lights_len = serialization_deserialize_u32(buffer, offset);
  P_U32(point_lights_len);

  for (u32 i = 0; i < point_lights_len; ++i)
  {
    serialization_deserialize_point_light(buffer, offset);
  } 
  
  SERIALIZATION_P("[serialization] deserialized scene");
}

// ---- terrain ----

void serialization_write_terrain_to_file(const char* name)
{
  u8* buffer = NULL;

  serialization_serialize_terrain(&buffer);

  char path[128];
  sprintf(path, ASSET_PATH"%s", name);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}
void serialization_load_terrain_from_file(const char* name)
{
  u32 offset = 0;
  int length = 0;
  
  char path[128];
  sprintf(path, ASSET_PATH"%s", name);
  u8* buffer = (u8*)file_read_bytes(path, &length);
  
  serialization_deserialize_terrain(buffer, &offset);

  free(buffer);
}
void serialization_serialize_terrain(u8** buffer)
{
  core_data = core_data_get();

  serialization_serialize_f32(buffer, core_data->terrain_scl);
  serialization_serialize_f32(buffer, core_data->terrain_y_scl);
  serialization_serialize_u32(buffer, core_data->terrain_x_len);
  serialization_serialize_u32(buffer, core_data->terrain_z_len);

  serialization_serialize_u32(buffer, core_data->terrain_layout_len);
  
  for (u32 i = 0; i < core_data->terrain_layout_len; ++i)
  {
    serialization_serialize_terrain_layout(buffer, &core_data->terrain_layout[i]);
  }
  SERIALIZATION_P("[serialization] serialized terrain");
}
void serialization_deserialize_terrain(u8* buffer, u32* offset)
{
  core_data = core_data_get();

  core_data->terrain_scl   = serialization_deserialize_f32(buffer, offset); 
  core_data->terrain_y_scl = serialization_deserialize_f32(buffer, offset);  
  core_data->terrain_x_len = serialization_deserialize_u32(buffer, offset); 
  core_data->terrain_z_len = serialization_deserialize_u32(buffer, offset); 

  core_data->terrain_layout = NULL;
  core_data->terrain_layout_len = serialization_deserialize_u32(buffer, offset);
  for (u32 i = 0; i < core_data->terrain_layout_len; ++i)
  {
    terrain_layout_t l;
    l.pos[0] = 0; l.pos[1] = 0; // vs doesnt like uninitialized struct l
    arrput(core_data->terrain_layout, l);
    serialization_deserialize_terrain_layout(buffer, offset, &core_data->terrain_layout[i]);
  }
  SERIALIZATION_P("[serialization] deserialized terrain");
}

// ---- complex types ----

void serialization_serialize_terrain_layout(u8** buffer, terrain_layout_t* l)
{
  u32 height_len = TERRAIN_LAYOUT_VERT_INFO_LEN(core_data);
  
  serialization_serialize_ivec2(buffer, l->pos);
  for (u32 i = 0; i < height_len; ++i)
  {
    serialization_serialize_f32(buffer, l->vert_info[i][0]); // height
    serialization_serialize_f32(buffer, l->vert_info[i][1]); // material
  }

}
void serialization_deserialize_terrain_layout(u8* buffer, u32* offset, terrain_layout_t* l)
{
  u32 height_len = TERRAIN_LAYOUT_VERT_INFO_LEN(core_data);
  
  serialization_deserialize_ivec2(buffer, offset, l->pos);
  l->vert_info = malloc(height_len * sizeof(vec2));
  for (u32 i = 0; i < height_len; ++i)
  {
    l->vert_info[i][0] = serialization_deserialize_f32(buffer, offset);  // height
    l->vert_info[i][1] = serialization_deserialize_f32(buffer, offset);  // material
  }
}

void serialization_serialize_entity(u8** buffer, entity_t* e)
{ 
  if (e->is_dead) { return; }
  serialization_serialize_s32(buffer, e->table_idx);

  serialization_serialize_vec3(buffer, e->pos);  
  serialization_serialize_vec3(buffer, e->rot);   
  serialization_serialize_vec3(buffer, e->scl);  

  serialization_serialize_s32(buffer, e->parent); 
  serialization_serialize_s32(buffer, e->children_len);
  for (u32 i = 0; i < e->children_len; ++i)
  {
    serialization_serialize_s32(buffer, e->children[i]);
  }
}
void serialization_deserialize_entity(u8* buffer, u32* offset)
{
  int table_idx = serialization_deserialize_s32(buffer, offset);

  vec3 pos, rot, scl;
  serialization_deserialize_vec3(buffer, offset, pos); 
  serialization_deserialize_vec3(buffer, offset, rot); 
  serialization_deserialize_vec3(buffer, offset, scl); 

  int idx = state_add_entity_from_template(pos, rot, scl, table_idx);
 
  bool error = false;
  entity_t* e = state_get_entity(idx, &error); ASSERT(!error);

  e->parent = serialization_deserialize_s32(buffer, offset);        
  e->children_len = serialization_deserialize_s32(buffer, offset);  
  for (u32 i = 0; i < e->children_len; ++i)
  {
    arrput(e->children, serialization_deserialize_s32(buffer, offset));
  }
  // PF("id: %d, parent: %d, children_len: %d\n", e->id, e->parent, e->children_len);
}

void serialization_serialize_dir_light(u8** buffer, dir_light_t* l)
{ 
  serialization_serialize_vec3(buffer, l->pos);
  serialization_serialize_vec3(buffer, l->dir);
  serialization_serialize_vec3(buffer, l->color);
  
  serialization_serialize_f32(buffer, l->intensity);

  serialization_serialize_u8(buffer, l->cast_shadow);
  serialization_serialize_s32(buffer, l->shadow_map_x);
  serialization_serialize_s32(buffer, l->shadow_map_y);
}
void serialization_deserialize_dir_light(u8* buffer, u32* offset)
{
  vec3 pos, dir, color;
  serialization_deserialize_vec3(buffer, offset, pos);
  serialization_deserialize_vec3(buffer, offset, dir);
  serialization_deserialize_vec3(buffer, offset, color);
  
  f32 intensity = serialization_deserialize_f32(buffer, offset);

  bool cast_shadow = serialization_deserialize_u8(buffer, offset);
  int shadow_map_x = serialization_deserialize_s32(buffer, offset);
  int shadow_map_y = serialization_deserialize_s32(buffer, offset);

  state_add_dir_light(pos, dir, color, intensity, cast_shadow, shadow_map_x, shadow_map_y);
}

void serialization_serialize_point_light(u8** buffer, point_light_t* l)
{ 
  serialization_serialize_vec3(buffer, l->pos);
  serialization_serialize_vec3(buffer, l->color);
  serialization_serialize_f32(buffer, l->intensity);
}
void serialization_deserialize_point_light(u8* buffer, u32* offset)
{
  vec3 pos, color;
  serialization_deserialize_vec3(buffer, offset, pos);
  serialization_deserialize_vec3(buffer, offset, color);
  f32 intensity = serialization_deserialize_f32(buffer, offset);

  state_add_point_light(pos, color, intensity);
}

// ---- base types ----

void serialization_serialize_u8(u8** buffer, u8 val)
{
  arrput(*buffer, (u8)(val));
}
u8 serialization_deserialize_u8(u8* buffer, u32* offset)
{
  u8 rtn = buffer[*offset];
  *offset += 1;
  return rtn;
}

void serialization_serialize_u32(u8** buffer, u32 val)
{
  arrput(*buffer, (u8)(val >> 24));
  arrput(*buffer, (u8)(val >> 16));
  arrput(*buffer, (u8)(val >> 8));
  arrput(*buffer, (u8)(val));
}
u32 serialization_deserialize_u32(u8* buffer, u32* offset)
{
  u8* data = buffer + *offset;
  u32 rtn  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  *offset += 4;
  return rtn;
}

void serialization_serialize_s32(u8** buffer, int val)
{
  arrput(*buffer, val >> 24);
  arrput(*buffer, val >> 16);
  arrput(*buffer, val >> 8);
  arrput(*buffer, val); 
}
int serialization_deserialize_s32(u8* buffer, u32* offset)
{
  u8* data = buffer + *offset;
  int rtn  = ((int)data[3]) + ((int)data[2] << 8) + ((int)data[1] << 16) + ((int)data[0] << 24);
  
  *offset += 4;
  return rtn;
}

void serialization_serialize_f32(u8** buffer, f32 val)
{
  f32_wrapper_t value;
  value.f_val = val;
  arrput(*buffer, value.u_val >> 24);
  arrput(*buffer, value.u_val >> 16);
  arrput(*buffer, value.u_val >> 8);
  arrput(*buffer, value.u_val);  
}
f32 serialization_deserialize_f32(u8* buffer, u32* offset)
{
  u8* data = buffer + *offset;
  f32_wrapper_t value;
  value.u_val = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  *offset += 4;
  return value.f_val;
}

void serialization_serialize_vec2(u8** buffer, vec2 val)
{
  serialization_serialize_f32(buffer, val[0]);
  serialization_serialize_f32(buffer, val[1]);
}
void serialization_deserialize_vec2(u8* buffer, u32* offset, vec2 out)
{
  out[0] = serialization_deserialize_f32(buffer, offset);
  out[1] = serialization_deserialize_f32(buffer, offset);
}
void serialization_serialize_ivec2(u8** buffer, ivec2 val)
{
  serialization_serialize_s32(buffer, val[0]);
  serialization_serialize_s32(buffer, val[1]);
}
void serialization_deserialize_ivec2(u8* buffer, u32* offset, ivec2 out)
{
  out[0] = serialization_deserialize_s32(buffer, offset);
  out[1] = serialization_deserialize_s32(buffer, offset);
}

void serialization_serialize_vec3(u8** buffer, vec3 val)
{
  serialization_serialize_f32(buffer, val[0]);
  serialization_serialize_f32(buffer, val[1]);
  serialization_serialize_f32(buffer, val[2]);
}
void serialization_deserialize_vec3(u8* buffer, u32* offset, vec3 out)
{
  out[0] = serialization_deserialize_f32(buffer, offset);
  out[1] = serialization_deserialize_f32(buffer, offset);
  out[2] = serialization_deserialize_f32(buffer, offset);
}

void serialization_serialize_str(u8** buffer, char* val)
{
  u32 len = strlen(val);
  P_U32(len);
  serialization_serialize_u32(buffer, len);

  for (int i = 0; i < len; ++i)
  {
    serialization_serialize_u8(buffer, val[i]); 
  }
}
// return needs to be copies as it gets overwritten next call
char* serialization_deserialize_str(u8* buffer, u32* offset)
{
  u32 len = serialization_deserialize_u32(buffer, offset);
  P_U32(len);
  for (int i = 0; i < len; ++i)
  {
    str_buf[i] = serialization_deserialize_u8(buffer, offset); 
  }
  str_buf[len] = '\0';
  return str_buf;
}

