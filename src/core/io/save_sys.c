#include "core/io/save_sys.h"
#include "core/core_data.h"
#include "core/state.h"
#include "core/io/assetm.h"
#include "core/io/file_io.h"
#include "core/types/cubemap.h"
#include "core/debug/debug_timer.h"
#include "serialization/serialization.h"

#include "core/camera.h"

#include "stb/stb_ds.h"


#define CUR_SCENE_NAME_MAX 128
char cur_scene_name[CUR_SCENE_NAME_MAX] = "";

u8* state_buffer = NULL;
char state_buffer_scene_name[CUR_SCENE_NAME_MAX] = "";

vec3 state_cam_pos         = { 0, 0, 0 };
vec3 state_cam_orientation = { 0, 0, 0 };

static core_data_t* core_data = NULL;


void save_sys_init()
{
  core_data = core_data_get();
}

int cur_version = 0;

// ---- structures ----

#ifdef EDITOR
void save_sys_write_structure_to_file(const char* name, int root_entity_id)
{
  u8* buffer = NULL;
   
  entity_t* root = state_get_entity(root_entity_id);
  
  u32 len = 0;
  state_get_entity_total_children_len(root_entity_id, &len);
  len++; // for root entity
  serialization_serialize_u32(&buffer, len); // structure length, amount of entities
  // P_U32(len);
  
  u32* entity_idxs = NULL; 
  u32  entity_idxs_pos = 0;
  MALLOC(entity_idxs, len * sizeof(u32));
  
  save_sys_get_structure_idxs(entity_idxs, &entity_idxs_pos, root);

  entity_t _root = *root;
  vec3_copy(VEC3(0), _root.pos);
  save_sys_serialize_structure(&buffer, entity_idxs, len, &_root);

  FREE(entity_idxs);

  SERIALIZATION_P("[serialization] serialized structure");

  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s%s%s", core_data->asset_path, "/structures/", name, ".struct");
  file_io_write(path, (const char*)buffer, (int)arrlen(buffer));

  ARRFREE(buffer);
}
void save_sys_get_structure_idxs(u32* arr, u32* arr_pos, entity_t* e)
{
  // PF("-> structure[%d] id: %d\n", (*arr_pos), root->id);
  arr[(*arr_pos)++] = e->id;

  for (u32 i = 0; i < e->children_len; ++i)
  {
    entity_t* c = state_get_entity(e->children[i]);
    save_sys_get_structure_idxs(arr, arr_pos, c);
  }
}
void save_sys_serialize_structure(u8** buffer, u32* idxs, u32 idxs_len, entity_t* e)
{
  // serialize children len
  //  - each child as index into the structure
  // serialize entity
  // -> recursion

  serialization_serialize_u32(buffer, e->children_len); // structure length, amount of entities
  for (u32 j = 0; j < e->children_len; ++j)
  {
    for (u32 k = 0; k < idxs_len; ++k)
    {
      if (idxs[k] == e->children[j])
      { 
        // PF(" -> entity: %d -> child: %d\n", root->id, idxs[k]);
        serialization_serialize_u32(buffer, k); 
      }  // serialize index of child into structure 
    }
  }
  entity_t _e = *e;     // copy entity, to not modify original
  _e.children_len =  0;
  _e.parent       = -1;
  save_sys_serialize_entity(buffer, &_e);

  for (u32 i = 0; i < e->children_len; ++i)
  {
    entity_t* c = state_get_entity(e->children[i]);  
    save_sys_serialize_structure(buffer, idxs, idxs_len, c); // use recursion
  }
}
int save_sys_load_structure_from_file(const char* name)
{
  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s%s%s", core_data->asset_path, "/structures/", name, ".struct");
  int file_len = 0;
  u8* buffer = (u8*)file_io_read_bytes(path, &file_len);
  u32 offset = 0;

  u32 len = serialization_deserialize_u32(buffer, &offset); // structure length, amount of entities
  
  // child_idxs[i / entity idxs in structure][0] = length of [][X] array
  // child_idxs[i / entity idxs in structure][X] = e.children[X-1] of entity
#ifndef _MSC_VER
  const int CHILD_IDXS_MAX_ENTITIES     = 64; ASSERT(len < CHILD_IDXS_MAX_ENTITIES);
  const int CHILD_IDXS_MAX_IDXS_INC_LEN = 64;
  const int CHILD_IDXS_MAX_IDXS         = CHILD_IDXS_MAX_IDXS_INC_LEN -1;
#else
  #define CHILD_IDXS_MAX_ENTITIES 64 
  ASSERT(len < CHILD_IDXS_MAX_ENTITIES);
  #define CHILD_IDXS_MAX_IDXS_INC_LEN 64
  #define CHILD_IDXS_MAX_IDXS CHILD_IDXS_MAX_IDXS_INC_LEN - 1
#endif
  u32 child_idxs[CHILD_IDXS_MAX_ENTITIES][CHILD_IDXS_MAX_IDXS_INC_LEN];
  u32 entity_ids[CHILD_IDXS_MAX_ENTITIES]; 

  int rtn_id = -1;
    
  // deserealize entities
  for (u32 i = 0; i < len; ++i)
  {
    u32 c_len = serialization_deserialize_u32(buffer, &offset); // amount of child idxs
    ASSERT(c_len < CHILD_IDXS_MAX_IDXS);
    child_idxs[i][0] = c_len;
  
    for (u32 j = 1; j < c_len +1; ++j) // 0 is len, tsart at 1
    {
      child_idxs[i][j] = serialization_deserialize_u32(buffer, &offset);
    }

    entity_ids[i] = save_sys_deserialize_entity(buffer, &offset);
    if (rtn_id < 0) { rtn_id = entity_ids[i]; }
    // P_U32(entity_ids[i]);
  }
  // parent entities
  for (u32 i = 0; i < len; ++i)
  {
    u32 c_len = child_idxs[i][0];

    for (u32 j = 1; j < c_len +1; ++j) // 0 is len, tsart at 1
    {
      // PF("-> parented: parent: %d, child: %d\n", entity_ids[i], entity_ids[child_idxs[i][j]]);
      state_entity_add_child(entity_ids[i], entity_ids[child_idxs[i][j]], false);
    }

  }

  FREE(buffer);

  return rtn_id;
}
#endif

// ---- scene ----

#ifdef EDITOR
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
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s", core_data->asset_path, SAVE_SYS_EMPTY_SCENE_NAME);
  file_io_write(path, (const char*)buffer, (int)arrlen(buffer));

  ARRFREE(buffer);
}
#endif

void save_sys_write_scene_to_file(const char* name)
{
  u8* buffer = NULL;

  save_sys_serialize_scene(&buffer);
  
  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s", core_data->asset_path, name);
  file_io_write(path, (const char*)buffer, (int)arrlen(buffer));

  ARRFREE(buffer);
}

void save_sys_load_scene_from_file_dbg(const char* name, const char* _file, const int _line)
{
  u32 offset = 0;
  int length = 0;

  // PF("save_sys_load_scene_from_file() called from\n -> \"%s\", line: %d\n", _file, _line);
  P_STR(core_data->asset_path);
  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX +64, path, "%s%s", core_data->asset_path, name);
  u8* buffer = (u8*)file_io_read_bytes(path, &length);
  save_sys_deserialize_scene(buffer, &offset);

  ASSERT(strlen(name) < CUR_SCENE_NAME_MAX);
  strcpy(cur_scene_name, name);

  FREE(buffer);
}

void save_sys_write_scene_to_state_buffer()
{
  u8* buffer = NULL;

  save_sys_serialize_scene(&buffer);
 
  // @TODO: 
  // !!! DO THIS NEXT
  // also add searching .h files for func-defs to term_docs

  REALLOC(state_buffer, arrlen(buffer) * sizeof(u8));
  memcpy(state_buffer, buffer, arrlen(buffer) * sizeof(u8));

  strcpy(state_buffer_scene_name, cur_scene_name);

  ARRFREE(buffer);

  vec3_copy(core_data->cam.pos,   state_cam_pos);
  vec3_copy(core_data->cam.front, state_cam_orientation);
  // camera_get_pos(state_cam_pos);
  // camera_get_front(state_cam_orientation);
}

void save_sys_load_scene_from_state_buffer()
{
  u32 offset = 0;
  
  save_sys_deserialize_scene(state_buffer, &offset);

  strcpy(cur_scene_name, state_buffer_scene_name);

  camera_set_pos(state_cam_pos);
  camera_set_front(state_cam_orientation);
}

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
  
  // @NOTE: now get serialized with entity
  // -- point lights --

  // int point_lights_len = 0;
  // point_light_t* point_lights = state_get_point_light_arr(&point_lights_len);
  //  
  // serialization_serialize_u32(buffer, point_lights_len);

  // for (u32 i = 0; i < point_lights_len; ++i)
  // {
  //   save_sys_serialize_point_light(buffer, &point_lights[i]);
  // } 
  
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
  f32 intensity = serialization_deserialize_f32(buffer, offset);
  char* cube_map_name = serialization_deserialize_str(buffer, offset);
  TIMER_FUNC_STATIC(core_data->cube_map = cubemap_load(cube_map_name));
  core_data->cube_map.intensity = intensity;

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

  // @NOTE: now get serialized with entities
  // -- point lights --
  
  // @BUGG: previous pointlight entites get serialized & deserialized
  //        then deserilaize_pointlight adds new entities

  // u32 point_lights_len = serialization_deserialize_u32(buffer, offset);
  // P_U32(point_lights_len);

  // for (u32 i = 0; i < point_lights_len; ++i)
  // {
  //   save_sys_deserialize_point_light(buffer, offset);
  // } 
  
  SERIALIZATION_P("[serialization] deserialized scene");
}

// ---- terrain ----

void save_sys_write_terrain_to_file(const char* name)
{
  u8* buffer = NULL;

  save_sys_serialize_terrain(&buffer);

  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s", core_data->asset_path, name);
  file_io_write(path, (const char*)buffer, (int)arrlen(buffer));

  ARRFREE(buffer);
}
void save_sys_load_terrain_from_file(const char* name)
{
  u32 offset = 0;
  int length = 0;
  
  char path[ASSET_PATH_MAX +64];
  SPRINTF(ASSET_PATH_MAX + 64, path, "%s%s", core_data->asset_path, name);
  u8* buffer = (u8*)file_io_read_bytes(path, &length);
  
  save_sys_deserialize_terrain(buffer, &offset);

  FREE(buffer);
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
  MALLOC(l->vert_info, height_len * sizeof(vec2));
  for (u32 i = 0; i < height_len; ++i)
  {
    l->vert_info[i][0] = serialization_deserialize_f32(buffer, offset);  // height
    l->vert_info[i][1] = serialization_deserialize_f32(buffer, offset);  // material
  }
}

void save_sys_serialize_entity(u8** buffer, entity_t* e)
{ 
  if (e->is_dead) { return; }
  serialization_serialize_s32(buffer, e->template_idx);

  serialization_serialize_vec3(buffer, e->pos);  
  serialization_serialize_vec3(buffer, e->rot);   
  serialization_serialize_vec3(buffer, e->scl);  

  serialization_serialize_u8(buffer, e->point_light_idx >= 0 ? 1 : 0);  // if has point light
  if (e->point_light_idx >= 0)
  {
    bool error = false;
    point_light_t* l = state_get_point_light(e->point_light_idx, &error); ASSERT(!error);
    save_sys_serialize_point_light(buffer, l);
  }

  serialization_serialize_s32(buffer, e->parent); 
  serialization_serialize_s32(buffer, e->children_len);
  for (u32 i = 0; i < e->children_len; ++i)
  {
    serialization_serialize_s32(buffer, e->children[i]);
  }
}
int save_sys_deserialize_entity(u8* buffer, u32* offset)
{
  int template_idx = serialization_deserialize_s32(buffer, offset);

  vec3 pos, rot, scl;
  serialization_deserialize_vec3(buffer, offset, pos); 
  serialization_deserialize_vec3(buffer, offset, rot); 
  serialization_deserialize_vec3(buffer, offset, scl); 

  int id = state_add_entity_from_template(pos, rot, scl, template_idx);
 
  entity_t* e = state_get_entity(id);
   
  u8 has_point_light = serialization_deserialize_u8(buffer, offset);  // if has point light
  if (has_point_light)
  {
    e->point_light_idx = save_sys_deserialize_point_light(buffer, offset, id);
  }

  e->parent = serialization_deserialize_s32(buffer, offset);        
  e->children_len = serialization_deserialize_s32(buffer, offset);  
  for (u32 i = 0; i < e->children_len; ++i)
  {
    arrput(e->children, serialization_deserialize_s32(buffer, offset));
  }
  // PF("id: %d, parent: %d, children_len: %d\n", e->id, e->parent, e->children_len);

  return id;
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
  serialization_serialize_vec3(buffer, l->offset);
  serialization_serialize_vec3(buffer, l->color);
  serialization_serialize_f32(buffer, l->intensity);
  // entity_id gets set in entity_deserealize
}
int save_sys_deserialize_point_light(u8* buffer, u32* offset, int entity_id)
{
  vec3 _offset, color;
  serialization_deserialize_vec3(buffer, offset, _offset);
  serialization_deserialize_vec3(buffer, offset, color);
  f32 intensity = serialization_deserialize_f32(buffer, offset);

  return state_add_point_light(_offset, color, intensity, entity_id);
}


