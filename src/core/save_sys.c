#include "core/save_sys.h"
#include "core/core_data.h"
#include "core/state.h"
#include "core/file_io.h"
#include "core/assetm.h"
#include "core/debug/debug_timer.h"
#include "serialization/serialization.h"

#ifdef EDITOR
#include "core/camera.h"
#endif

#include "stb/stb_ds.h"


#define CUR_SCENE_NAME_MAX 128
char cur_scene_name[CUR_SCENE_NAME_MAX] = "";

#ifdef EDITOR
u8* state_buffer = NULL;
char state_buffer_scene_name[CUR_SCENE_NAME_MAX] = "";

vec3 state_cam_pos         = { 0, 0, 0 };
vec3 state_cam_orientation = { 0, 0, 0 };
#endif

static core_data_t* core_data = NULL;


void save_sys_init()
{
  core_data = core_data_get();
}

int cur_version = 0;


// ---- scene ----

void save_sys_write_scene_to_file(const char* name)
{
  u8* buffer = NULL;

  save_sys_serialize_scene(&buffer);
  
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%s%s", core_data->asset_path, name);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));


  arrfree(buffer);
}

void save_sys_load_scene_from_file(const char* name)
{
  u32 offset = 0;
  int length = 0;
 
  if (core_data == NULL) { ERR("u stupid\n") };
  P_STR(core_data->asset_path);
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%s%s", core_data->asset_path, name);
  u8* buffer = (u8*)file_read_bytes(path, &length);
  
  save_sys_deserialize_scene(buffer, &offset);

  ASSERT(strlen(name) < CUR_SCENE_NAME_MAX);
  strcpy(cur_scene_name, name);

  free(buffer);
}

#ifdef EDITOR
void save_sys_write_scene_to_state_buffer()
{
  u8* buffer = NULL;

  save_sys_serialize_scene(&buffer);
 
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

void save_sys_load_scene_from_state_buffer()
{
  u32 offset = 0;
  
  save_sys_deserialize_scene(state_buffer, &offset);

  strcpy(cur_scene_name, state_buffer_scene_name);

  camera_set_pos(state_cam_pos);
  camera_set_front(state_cam_orientation);
}

void save_sys_write_empty_scene_to_file()
{
  u8* buffer = NULL;

  serialization_serialize_u32(&buffer, SAVE_SYS_VERSION);
   
  // -- cubemap --
  serialization_serialize_f32(&buffer, 1.0f);  // cube_map.intensity
  serialization_serialize_str(&buffer, "#cubemaps/gothic_manor_01_2k.hdr"); // cubemap.name
  
  // serialization_serialize_scene(&buffer);
  serialization_serialize_u32(&buffer, 0); // world_len
  serialization_serialize_u32(&buffer, 0); // dir_lights_len
  serialization_serialize_u32(&buffer, 0); // point_lights_len
  
  SERIALIZATION_P("[serialization] serialized empty scene");

  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%s%s", core_data->asset_path, "-_-_new_-_-");
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}
#endif

void save_sys_serialize_scene(u8** buffer)
{
  // -- version --

  serialization_serialize_u32(buffer, SAVE_SYS_VERSION);
   
  // -- cubemap --
  serialization_serialize_f32(buffer, core_data->cube_map.intensity);
  serialization_serialize_str(buffer, core_data->cube_map.name); 
  
  // -- entities --

  int world_len = 0;
  int world_dead_len = 0;
  entity_t* world = state_get_entity_arr(&world_len, &world_dead_len);

  serialization_serialize_u32(buffer, world_len - world_dead_len);

  for (u32 i = 0; i < world_len; ++i)
  {
    if (world[i].is_dead) { continue; }
    save_sys_serialize_entity(buffer, &world[i]);
  }

  // -- dir lights --
  
  int dir_lights_len = 0;
  dir_light_t* dir_lights = state_get_dir_light_arr(&dir_lights_len);
   
  serialization_serialize_u32(buffer, dir_lights_len);

  for (u32 i = 0; i < dir_lights_len; ++i)
  {
    save_sys_serialize_dir_light(buffer, &dir_lights[i]);
  } 
   
  // -- point lights --

  int point_lights_len = 0;
  point_light_t* point_lights = state_get_point_light_arr(&point_lights_len);
   
  serialization_serialize_u32(buffer, point_lights_len);

  for (u32 i = 0; i < point_lights_len; ++i)
  {
    save_sys_serialize_point_light(buffer, &point_lights[i]);
  } 
  
  SERIALIZATION_P("[serialization] serialized scene");
}
void save_sys_deserialize_scene(u8* buffer, u32* offset)
{

  // clear pre-existing scene
  state_clear_scene();
 
  // -- version -- 
  
  cur_version = serialization_deserialize_u32(buffer, offset);
  PF("| serialization version: %d\n", cur_version);

  // -- cubemap --
  if (cur_version >= 2)
  {
    f32 intensity = serialization_deserialize_f32(buffer, offset);
    char* cube_map_name = serialization_deserialize_str(buffer, offset);
    TIMER_FUNC_STATIC(core_data->cube_map = assetm_load_cubemap_hdr(cube_map_name));
    core_data->cube_map.intensity = intensity;
  }
  else
  {
    TIMER_FUNC_STATIC(core_data->cube_map = assetm_load_cubemap_hdr("#cubemaps/gothic_manor_01_2k.hdr"));
  }

  // -- entities --

  u32 world_len = serialization_deserialize_u32(buffer, offset);
  P_U32(world_len);

  for (u32 i = 0; i < world_len; ++i)
  {
    save_sys_deserialize_entity(buffer, offset);
  }
  
  // -- dir lights --
  
  u32 dir_lights_len = serialization_deserialize_u32(buffer, offset);
  P_U32(dir_lights_len);

  for (u32 i = 0; i < dir_lights_len; ++i)
  {
    save_sys_deserialize_dir_light(buffer, offset);
  }

  // -- point lights --
   
  u32 point_lights_len = serialization_deserialize_u32(buffer, offset);
  P_U32(point_lights_len);

  for (u32 i = 0; i < point_lights_len; ++i)
  {
    save_sys_deserialize_point_light(buffer, offset);
  } 
  
  SERIALIZATION_P("[serialization] deserialized scene");
}

// ---- terrain ----

void save_sys_write_terrain_to_file(const char* name)
{
  u8* buffer = NULL;

  save_sys_serialize_terrain(&buffer);

  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%s%s", core_data->asset_path, name);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}
void save_sys_load_terrain_from_file(const char* name)
{
  u32 offset = 0;
  int length = 0;
  
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%s%s", core_data->asset_path, name);
  u8* buffer = (u8*)file_read_bytes(path, &length);
  
  save_sys_deserialize_terrain(buffer, &offset);

  free(buffer);
}
void save_sys_serialize_terrain(u8** buffer)
{
  serialization_serialize_f32(buffer, core_data->terrain_scl);
  serialization_serialize_f32(buffer, core_data->terrain_y_scl);
  serialization_serialize_u32(buffer, core_data->terrain_x_len);
  serialization_serialize_u32(buffer, core_data->terrain_z_len);

  serialization_serialize_u32(buffer, core_data->terrain_layout_len);
  
  for (u32 i = 0; i < core_data->terrain_layout_len; ++i)
  {
    save_sys_serialize_terrain_layout(buffer, &core_data->terrain_layout[i]);
  }
  SERIALIZATION_P("[serialization] serialized terrain");
}
void save_sys_deserialize_terrain(u8* buffer, u32* offset)
{
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
    save_sys_deserialize_terrain_layout(buffer, offset, &core_data->terrain_layout[i]);
  }
  SERIALIZATION_P("[serialization] deserialized terrain");
}

// ---- complex types ----

void save_sys_serialize_terrain_layout(u8** buffer, terrain_layout_t* l)
{
  u32 height_len = TERRAIN_LAYOUT_VERT_INFO_LEN(core_data);
  
  serialization_serialize_ivec2(buffer, l->pos);
  for (u32 i = 0; i < height_len; ++i)
  {
    serialization_serialize_f32(buffer, l->vert_info[i][0]); // height
    serialization_serialize_f32(buffer, l->vert_info[i][1]); // material
  }

}
void save_sys_deserialize_terrain_layout(u8* buffer, u32* offset, terrain_layout_t* l)
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

void save_sys_serialize_entity(u8** buffer, entity_t* e)
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
void save_sys_deserialize_entity(u8* buffer, u32* offset)
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

void save_sys_serialize_dir_light(u8** buffer, dir_light_t* l)
{ 
  serialization_serialize_vec3(buffer, l->pos);
  serialization_serialize_vec3(buffer, l->dir);
  serialization_serialize_vec3(buffer, l->color);
  
  serialization_serialize_f32(buffer, l->intensity);

  serialization_serialize_u8(buffer, l->cast_shadow);
  serialization_serialize_s32(buffer, l->shadow_map_x);
  serialization_serialize_s32(buffer, l->shadow_map_y);
}
void save_sys_deserialize_dir_light(u8* buffer, u32* offset)
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

void save_sys_serialize_point_light(u8** buffer, point_light_t* l)
{ 
  serialization_serialize_vec3(buffer, l->pos);
  serialization_serialize_vec3(buffer, l->color);
  serialization_serialize_f32(buffer, l->intensity);
}
void save_sys_deserialize_point_light(u8* buffer, u32* offset)
{
  vec3 pos, color;
  serialization_deserialize_vec3(buffer, offset, pos);
  serialization_deserialize_vec3(buffer, offset, color);
  f32 intensity = serialization_deserialize_f32(buffer, offset);

  state_add_point_light_empty(pos, color, intensity);
}


