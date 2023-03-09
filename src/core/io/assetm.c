#include "core/io/assetm.h"
#include "core/io/file_io.h"
#include "core/io/asset_io.h"
#include "core/core_data.h"
#include "core/types/shader.h"
#include "core/types/mesh.h"
#include "core/types/material.h"
#include "data/material_template.h"
#include "core/debug/debug_timer.h"
#include "math/math_inc.h"

#include "GLAD/glad.h"
#include "stb/stb_ds.h"
#include "stb/stb_image.h"
#include "zip/zip.h"

struct zip_t* zip_textures;
struct zip_t* zip_meshes;

#define MAX_ITEMS 8    

// ---- textures ----
// value: index of texture in 'tex', key: name of texture 
struct { char* key;  int value; }* texture_idxs = NULL;
int texture_idxs_len = 0;

// array holding textures
texture_t* texture_data = NULL;
int texture_data_len = 0;

// ---- meshes ----
// value: index of texture in 'tex', key: name of texture 
struct { char* key;  int value; }* mesh_idxs = NULL;
int mesh_idxs_len = 0;

// array holding textures
mesh_t* mesh_data = NULL;
int mesh_data_len = 0;

// ---- shaders ----
// value: index of texture in 'tex', key: name of texture 
struct { int key;  int value; }* shader_idxs = NULL;
int shader_idxs_len = 0;

// array holding textures
shader_t* shader_data = NULL;
int shader_data_len = 0;

// ---- materials ----
// value: index of texture in 'tex', key: name of texture 
struct { int key;  int value; }* material_idxs = NULL;
int material_idxs_len = 0;

// array holding textures
material_t* material_data = NULL;
int material_data_len = 0;



static core_data_t* core_data = NULL;

// #define MATERIALS_PATH core_data->asset_path"materials/"
// #define core_data->shaders_path   core_data->asset_path"shaders/"

void assetm_init()
{
#ifdef ASSETM_NO_ZIP
  ASSETM_P("-- ASSETM NO ZIP --");
#else 
  ASSETM_P("-- ASSETM ZIP --");
#endif

  core_data = core_data_get();

  // testing for guids
  // uint64_t g;
  // g = rand_u64(); P_U64(g);
 
  // set default return if key doesn't exist
  shdefault(texture_idxs, -1);
  shdefault(mesh_idxs, -1);
  hmdefault(shader_idxs, -1);
  hmdefault(material_idxs, -1);
  // set start length
  arrsetcap(texture_data,  MAX_ITEMS);
  arrsetcap(mesh_data,     MAX_ITEMS);
  arrsetcap(shader_data,   MAX_ITEMS);
  arrsetcap(material_data, MAX_ITEMS);

  // open zip archive
  char path[ASSET_PATH_MAX + 64];
  sprintf(path, "%s/textures.zip", core_data->asset_path); 
  zip_textures  = zip_open(path, 0, 'r');
  sprintf(path, "%s/meshes.zip", core_data->asset_path); 
  zip_meshes    = zip_open(path, 0, 'r');

}

void assetm_cleanup()
{
  zip_close(zip_textures);
  zip_close(zip_meshes);
  
  // free the allocated memory
  shfree(texture_idxs);
  arrfree(texture_data);
  shfree(mesh_idxs);
  arrfree(mesh_data);
  hmfree(shader_idxs);
  arrfree(shader_data);
  hmfree(material_idxs);
  arrfree(material_data);
}

// textures ---------------------------------------------------------------------------------------

texture_t* assetm_get_texture_by_idx(int idx)
{
  ASSERT(idx >= 0 && idx < texture_data_len);
  return &texture_data[idx];
}
int assetm_get_texture_idx_dbg(const char* name, bool srgb, const char* file, const int line)
{
  if (shget(texture_idxs, name) < 0) // @NOTE: changed from '<='
  {
    assetm_create_texture_dbg(name, srgb, file, line);
  }
  return shget(texture_idxs, name);
}
texture_t* assetm_get_texture_dbg(const char* name, bool srgb, const char* file, const int line)
{
  if (shget(texture_idxs, name) < 0) // @NOTE: changed from '<='
  {
    assetm_create_texture_dbg(name, srgb, file, line);
  }
  return &texture_data[shget(texture_idxs, name)];
}
void assetm_create_texture_dbg(const char* name, bool srgb, const char* file, const int line)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  ASSERT(name_cpy != NULL);
  strcpy(name_cpy, name);
  u32 name_cpy_len = strlen(name_cpy);
  char name_cpy_char = name_cpy[name_cpy_len -4];
  name_cpy[name_cpy_len -4] = '\0'; // term string before '.png' / '.jpg'
  // PF("[assetm_create_texture] "); P_STR(name_cpy);
  
  // load texture --------------------------------------------

  // TIMER_START("texture loading .tex");    
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%stextures/%s%s", core_data->asset_path, name_cpy, ".tex");
  // PF("[assetm_create_texture] "); P_STR(path);
  
#ifdef EDITOR
  if (!file_io_check_exists(path)) // .tex
  {
    asset_io_convert_texture(name);
    PF("| converted %s -> .tex\n", name);
  }
#endif

  texture_t t = asset_io_load_texture(name, srgb);
  // PF("|%s|", name); TIMER_STOP_PRINT();
  
  name_cpy[name_cpy_len -4] = name_cpy_char; // add ending back into name_cpy, for shmp

  //   void*  buf = NULL;
  //   size_t buf_len = 0;
  //   // TIMER_START(" -> load tex file");
  // #ifdef ASSETM_NO_ZIP
  //   char path[ASSET_PATH_MAX +64];
  //   int len = 0;
  //   sprintf(path, "%stextures/%s", core_data->asset_path, name);
  //   buf = (void*)file_io_read_len(path, &len);
  //   buf_len = len;
  //   ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_texture(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", name, file, line);
  // #else 
  //   zip_entry_open(zip_textures, name);
  //   {
  //     zip_entry_read(zip_textures, &buf, &buf_len);
  //   }
  //   zip_entry_close(zip_textures);
  //   ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_texture(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);
  // #endif
  // // TIMER_STOP_PRINT();

  // // TIMER_START(" -> stbi load");
  // stbi_set_flip_vertically_on_load(true);
  // int w, h, channels;
  // u8* pixels = stbi_load_from_memory(buf, buf_len, &w, &h, &channels, 0); //, STBI_rgb_alpha);
  // // TIMER_STOP_PRINT();
  // // TIMER_START(" -> create texture");
  // u32 handle = texture_create_from_pixels(pixels, w, h, channels, srgb);
  // // TIMER_STOP_PRINT();
  // // PF(" -> \"%s\"\n", name);

  // texture_t t;
  // t.handle = handle;
  // t.width = w;
  // t.height = h;

  // // free(pixels);
  // stbi_image_free(pixels);
  // free(buf);

  // ---------------------------------------------------------

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(texture_idxs, name_cpy, texture_data_len);
  arrput(texture_data, t);
  texture_data_len++;

  ASSETM_PF("[assetm] loaded texture '%s', handle: '%d'\n", name, handle);
  // PF("[assetm] created texture: %s\n  -> idx: %d\n", name, shget(texture_idxs, name));
}

void assetm_get_texture_data_dbg(const char* name, int* width, int* height, int* channel_num, u8** pixels, const char* file, const int line)
{
  void*  buf = NULL;
  size_t buf_len = 0;
#ifdef ASSETM_NO_ZIP
  char path[ASSET_PATH_MAX +64];
  int len = 0;
  sprintf(path, "%stextures/%s", core_data->asset_path, name);
  buf = (void*)file_io_read_len(path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in assetm_load_cubemap_hdr(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", path, file, line);
#else 
  zip_entry_open(zip_textures, name);
  {
    zip_entry_read(zip_textures, &buf, &buf_len);
  }
  zip_entry_close(zip_textures);
  ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_texture() / assetm_get_texture(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);
#endif
  
  stbi_set_flip_vertically_on_load(true);
  *pixels = stbi_load_from_memory(buf, buf_len, width, height, channel_num, STBI_rgb_alpha);

  free(buf);
}
int assetm_add_texture(texture_t* tex, const char* name)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  ASSERT(name_cpy != NULL);
  strcpy(name_cpy, name);
  
  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(texture_idxs, name_cpy, texture_data_len);
  arrput(texture_data, *tex);
  texture_data_len++;
  return texture_data_len -1;
}


// meshes -----------------------------------------------------------------------------------------

mesh_t* assetm_get_mesh_by_idx_dbg(int idx, const char* file, const int line)
{
  ERR_CHECK(idx >= 0 && idx < mesh_data_len, "called from file: '%s' line: %d\n", file, line);
  return &mesh_data[idx];
}
int assetm_get_mesh_idx_dbg(const char* name, const char* file, const int line)
{  
  if (shget(mesh_idxs, name) < 0) // @NOTE: changed from '<='
  {
    assetm_create_mesh_dbg(name, file, line);
  }
  return shget(mesh_idxs, name);
}
mesh_t* assetm_get_mesh_dbg(const char* name, const char* file, const int line)
{
  if (shget(mesh_idxs, name) < 0) // @NOTE: changed from '<='
  {
    assetm_create_mesh_dbg(name, file, line);
  }
  return &mesh_data[shget(mesh_idxs, name)];
}
void assetm_create_mesh_dbg(const char* name, const char* file, const int line)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  ASSERT(name_cpy != NULL);
  strcpy(name_cpy, name);
  
  // load mesh -----------------------------------------------

  // TIMER_START("mesh loading .mesh");    
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%smeshes/%s%s", core_data->asset_path, name, ".mesh");
  
#ifdef EDITOR
  if (!file_io_check_exists(path)) // .mesh
  {
    asset_io_convert_mesh(name);
    PF("| converted %s.fbx -> .mesh\n", name);
  }
#endif

  mesh_t mesh = asset_io_load_mesh(name);
  // PF("|%s|", name); TIMER_STOP_PRINT();

  // .mesh vs. .fbx is ~2.5x faster

  // -- old --
  // void*  buf = NULL;
  // size_t buf_len = 0;

  // TIMER_START("mesh loading .fbx");    
  // #ifdef ASSETM_NO_ZIP
  // char path[ASSET_PATH_MAX +64];
  // int len = 0;
  // sprintf(path, "%smeshes/%s.fbx", core_data->asset_path, name);
  // buf = (void*)file_io_read_len(path, &len);
  // buf_len = len;
  // #else
  // zip_entry_open(zip_meshes, name);
  // {
  //   zip_entry_read(zip_meshes, &buf, &buf_len);
  // }
  // zip_entry_close(zip_meshes);
  // #endif
  // ERR_CHECK(buf != NULL || buf_len != 0, "mesh '%s' requested in assetm_create_mesh() / assetm_get_mesh(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);

  // mesh_t mesh = mesh_load_from_memory(buf, buf_len, name);
  // free(buf);
  // PF("|%s|", name); TIMER_STOP_PRINT();

  // ---------------------------------------------------------

  ASSETM_PF("[assetm] loaded mesh '%s'\n", name);

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(mesh_idxs, name_cpy, mesh_data_len);
  arrput(mesh_data, mesh);
  mesh_data_len++;

  // PF("[assetm] created mesh: %s\n  -> name: %d\n", name, shget(mesh_idxs, name));
}
int assetm_add_mesh(mesh_t* mesh, const char* name)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  ASSERT(name_cpy != NULL);
  strcpy(name_cpy, name);
  
  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(mesh_idxs, name_cpy, mesh_data_len);
  arrput(mesh_data, *mesh);
  mesh_data_len++;
  return mesh_data_len -1;
}

// shaders ----------------------------------------------------------------------------------------

shader_t* assetm_get_shader_by_idx_dbg(int idx, const char* file, const int line)
{
  ERR_CHECK(idx >= 0 && idx < shader_data_len, "index: %d, shader_data_len: %d\n -> [FILE] '%s', [LINE] %d", idx, shader_data_len, file, line);
  return &shader_data[idx];
}
int assetm_get_shader_idx_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  if (hmget(shader_idxs, type) < 0) // @NOTE: changed from '<='
  {
    assetm_create_shader_dbg(type, file, line);
  }
  return hmget(shader_idxs, type);
}
shader_t* assetm_get_shader_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  if (hmget(shader_idxs, type) < 0) // @NOTE: changed from '<='
  {
    assetm_create_shader_dbg(type, file, line);
  }
  return &shader_data[hmget(shader_idxs, type)];
}
void assetm_create_shader_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  shader_t s = assetm_create_shader_from_template_dbg(type, file, line);

  ASSETM_PF("[assetm] loaded shader '%d'\n", type);

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  hmput(shader_idxs, type, shader_data_len);
  arrput(shader_data, s);
  shader_data_len++;
  
  // PF("[assetm] created shader: %d\n  -> idx: %d\n", type, hmget(shader_idxs, type));
}
shader_t assetm_create_shader_from_template_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  // void*  buf = NULL;
  // size_t buf_len = 0;

// #ifdef ASSETM_NO_ZIP
  // char path[128]; 
  // sprintf(path, core_data->shaders_path"%s", name);
  // buf = file_read_len(path, (int*)&buf_len);
  const shader_template_t* s = shader_template_get(type);

  char vert_path[ASSET_PATH_MAX +64]; 
  sprintf(vert_path, "%s%s", core_data->shaders_path, s->vert);
  char frag_path[ASSET_PATH_MAX +64]; 
  sprintf(frag_path, "%s%s", core_data->shaders_path,s->frag);

  return shader_create_from_file(vert_path, frag_path, s->set_uniforms_f, s->name);
	
// #else
//   ERR("zip loading for shaders not yet implemented\n -> [FILE] '%s', [LINE] %d\n", file, line);
//   // zip_entry_open(zip_materials, name);
//   // {
//   //   zip_entry_read(zip_materials, &buf, &buf_len);
//   // }
//   // zip_entry_close(zip_materials);
// #endif 
  // ERR_CHECK(buf != NULL || buf_len != 0, "material '%s' requested in assetm_create_material() / assetm_get_material(), doesn't exist in the zip archive or folder.", name);
  // free(buf);
  // printf("[assetm] loaded shader '%s'\n", name);
}


// materials --------------------------------------------------------------------------------------

// @TODO: use '_dbg' func's and macros for __FILE__ & __LINE__ as well 

material_t* assetm_get_material_by_idx(int idx)
{
  ASSERT(idx >= 0 && idx < material_data_len);
  return &material_data[idx];
}
int assetm_get_material_idx(material_template_type type)
{  
  if (hmget(material_idxs, type) < 0)
  {
    assetm_create_material(type); 
  }
  // else
  // {
  //   int idx = hmget(material_idxs, type);
  //   PF("[idx | %d] material_idx: %d\n", type, idx);
  // }
  return hmget(material_idxs, type);
}
// @BUGG: creates again each time is called
material_t* assetm_get_material(material_template_type type)
{
  if (hmget(material_idxs, type) < 0)
  {
    assetm_create_material(type); 
  }
  // int idx = hmget(material_idxs, type);
  // PF("[mat | %d] material_idx: %d\n", type, idx);
  // PF("[mat | %d] mat.albedo: %d\n", type, material_data[idx].albedo);
  return &material_data[hmget(material_idxs, type)];
}
void assetm_create_material(material_template_type type)
{
  const material_template_t* m = material_template_get(type);
  material_t mat = material_load_from_template(m);

  // ---------------------------------------------------------

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  hmput(material_idxs, type, material_data_len);
  arrput(material_data, mat);
  material_data_len++;

  ASSETM_PF("[assetm] loaded material '%d'\n", type);
  // int idx = hmget(material_idxs, type);
  // PF("[assetm] created material: %d\n  -> idx: %d\n", type, idx);
}
