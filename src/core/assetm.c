#include "core/assetm.h"
#include "core/file_io.h"
#include "core/core_data.h"
#include "core/types/shader.h"
#include "core/types/mesh.h"
#include "core/types/material.h"
#include "data/material_template.h"
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

// #define MATERIALS_PATH ASSET_PATH"materials/"
#define SHADERS_PATH   ASSET_PATH"shaders/"

void assetm_init()
{
#ifdef ASSETM_NO_ZIP
  P("-- ASSETM NO ZIP --");
#else 
  P("-- ASSETM ZIP --");
#endif

  core_data = core_data_get();

  // testing for guids
  // uint64_t g;
  // g = rand_u64(); P_U64(g);
 
  // set default return if key doesn't exist
  shdefault(texture_idxs, -1);
  shdefault(mesh_idxs, -1);
  // set start length
  arrsetcap(texture_data, MAX_ITEMS);
  arrsetcap(mesh_data, MAX_ITEMS);

  // open zip archive
  zip_textures  = zip_open(ASSET_PATH"textures/textures.zip", 0, 'r');
  zip_meshes    = zip_open(ASSET_PATH"meshes/meshes.zip", 0, 'r');

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
}

// textures ---------------------------------------------------------------------------------------

texture_t* assetm_get_texture_by_idx(int idx)
{
  assert(idx >= 0 && idx < texture_data_len);
  return &texture_data[idx];
}
int assetm_get_texture_idx_dbg(const char* name, bool srgb, const char* file, const int line)
{
  if (shget(texture_idxs, name) <= 0)
  {
    assetm_create_texture_dbg(name, srgb, file, line);
  }
  return shget(texture_idxs, name);
}
texture_t* assetm_get_texture_dbg(const char* name, bool srgb, const char* file, const int line)
{
  if (shget(texture_idxs, name) <= 0)
  {
    assetm_create_texture_dbg(name, srgb, file, line);
  }
  return &texture_data[shget(texture_idxs, name)];
}
void assetm_create_texture_dbg(const char* name, bool srgb, const char* file, const int line)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  assert(name_cpy != NULL);
  strcpy(name_cpy, name);
  
  // load texture --------------------------------------------

  void*  buf = NULL;
  size_t buf_len = 0;
#ifdef ASSETM_NO_ZIP
  char path[64];
  int len = 0;
  sprintf(path, ASSET_PATH"textures/%s", name);
  buf = (void*)file_read_len(path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_texture(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", name, file, line);
#else 
  zip_entry_open(zip_textures, name);
  {
    zip_entry_read(zip_textures, &buf, &buf_len);
  }
  zip_entry_close(zip_textures);
  ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_texture(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);
#endif

  stbi_set_flip_vertically_on_load(true);
  int w, h, channels;
  u8* pixels = stbi_load_from_memory(buf, buf_len, &w, &h, &channels, 0); //, STBI_rgb_alpha);
  u32 handle = texture_create_from_pixels(pixels, w, h, channels, srgb);

  texture_t t;
  t.handle = handle;
  t.width = w;
  t.height = h;
  PF("[assetm] loaded texture '%s', handle: '%d'\n", name, handle);

  // free(pixels);
  stbi_image_free(pixels);
  free(buf);

  // ---------------------------------------------------------

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(texture_idxs, name_cpy, texture_data_len);
  arrput(texture_data, t);
  texture_data_len++;
}

void assetm_get_texture_data_dbg(const char* name, int* width, int* height, int* channel_num, u8** pixels, const char* file, const int line)
{
  void*  buf = NULL;
  size_t buf_len = 0;
#ifdef ASSETM_NO_ZIP
  char path[64];
  int len = 0;
  sprintf(path, ASSET_PATH"textures/%s", name);
  buf = (void*)file_read_len(path, &len);
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

cubemap_t assetm_load_cubemap_hdr_dbg(const char* path, const char* file, const int line) 
{

  // fix seams betweencubemap faces
  // this shoud be activated in renderer
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  
  // load hdr image ----------------------------------------------------------------------
 
  void*  buf = NULL;
  size_t buf_len = 0;
#ifdef ASSETM_NO_ZIP
  char _path[64];
  int len = 0;
  sprintf(_path, ASSET_PATH"textures/%s", path);
  buf = (void*)file_read_len(_path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in assetm_load_cubemap_hdr(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", path, file, line);
#else 
  zip_entry_open(zip_textures, path);
  {
    zip_entry_read(zip_textures, &buf, &buf_len);
  }
  zip_entry_close(zip_textures);
  ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in assetm_load_cubemap_hdr(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", path, file, line);
#endif

  // stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  float *data = stbi_loadf_from_memory(buf, buf_len, &width, &height, &channels, 0);
  unsigned int hdr_texture;
  if (data)
  {
    glGenTextures(1, &hdr_texture);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  { ERR("failed to load HDR image."); }

  // gen framebuffer ---------------------------------------------------------------------
    
  unsigned int capture_fbo, capture_rbo;
  glGenFramebuffers(1, &capture_fbo);
  glGenRenderbuffers(1, &capture_rbo);

  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_fbo);

  // gen cubemap -------------------------------------------------------------------------
  
  unsigned int cubemap;
  glGenTextures(1, &cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  for (unsigned int i = 0; i < 6; ++i)
  {
    // note that we store each face with 16 bit floating point values
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
        512, 512, 0, GL_RGB, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // render cubemap ----------------------------------------------------------------------
 
  mesh_t* cube_mesh = assetm_get_mesh("cube.fbx");

  mat4 proj;
  float pers = 90.0f;
  m_deg_to_rad(&pers);
  mat4_perspective(pers, 1.0f, 0.1f, 10.0f, proj);
  mat4 view_mats[6]; 
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ( 1.0f,  0.0f,  0.0f), VEC3_XYZ(0.0f, -1.0f,  0.0f), view_mats[0]);
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ(-1.0f,  0.0f,  0.0f), VEC3_XYZ(0.0f, -1.0f,  0.0f), view_mats[1]);
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ( 0.0f,  1.0f,  0.0f), VEC3_XYZ(0.0f,  0.0f,  1.0f), view_mats[2]);
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ( 0.0f, -1.0f,  0.0f), VEC3_XYZ(0.0f,  0.0f, -1.0f), view_mats[3]);
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ( 0.0f,  0.0f,  1.0f), VEC3_XYZ(0.0f, -1.0f,  0.0f), view_mats[4]);
  mat4_lookat(VEC3_XYZ(0.0f, 0.0f, 0.0f), VEC3_XYZ( 0.0f,  0.0f, -1.0f), VEC3_XYZ(0.0f, -1.0f,  0.0f), view_mats[5]);
  
  // convert HDR equirectangular environment map to cubemap equivalent
  shader_use(&core_data->equirect_shader);
  shader_set_int(&core_data->equirect_shader, "equirect_map", 0);
  shader_set_mat4(&core_data->equirect_shader, "proj", proj);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_texture);

  glDisable(GL_CULL_FACE);
  glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  for (unsigned int i = 0; i < 6; ++i)
  {
    shader_set_mat4(&core_data->equirect_shader, "view", view_mats[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(cube_mesh->vao);
    if (cube_mesh->indexed)
    { glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
    else
    { glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  
  // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  
  // render irradiencemap ----------------------------------------------------------------

  unsigned int irradiance_map;
  glGenTextures(1, &irradiance_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
  for (unsigned int i = 0; i < 6; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, 
        GL_RGB, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);  

  shader_use(&core_data->irradiance_map_shader);
  shader_set_mat4(&core_data->irradiance_map_shader, "proj", proj);
  shader_set_int(&core_data->irradiance_map_shader, "environment_map", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

  glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  for (unsigned int i = 0; i < 6; ++i)
  {
    shader_set_mat4(&core_data->irradiance_map_shader, "view", view_mats[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(cube_mesh->vao);
    if (cube_mesh->indexed)
    { glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
    else
    { glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  
  // gen radiance map --------------------------------------------------------------------
  
  unsigned int prefilter_map;
  glGenTextures(1, &prefilter_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map);
  for (unsigned int i = 0; i < 6; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // render radiance map -----------------------------------------------------------------
  
  shader_use(&core_data->prefilter_shader);
  shader_set_mat4(&core_data->prefilter_shader, "proj", proj);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  shader_set_int(&core_data->prefilter_shader, "environment_map", 0);

  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  unsigned int max_mip_levels = 5;
  for (unsigned int mip = 0; mip < max_mip_levels; ++mip)
  {
    // reisze framebuffer according to mip-level size.
    unsigned int mip_w = 128 * pow(0.5, mip);
    unsigned int mip_h = 128 * pow(0.5, mip);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_w, mip_h);
    glViewport(0, 0, mip_w, mip_h);

    float roughness = (float)mip / (float)(max_mip_levels - 1);
    shader_set_float(&core_data->prefilter_shader, "roughness", roughness);
    for (unsigned int  i = 0; i < 6; ++i)
    {
      shader_set_mat4(&core_data->prefilter_shader, "view", view_mats[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map, mip);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glBindVertexArray(cube_mesh->vao);
      if (cube_mesh->indexed)
      { glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
      else
      { glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glEnable(GL_CULL_FACE);
  
  glDeleteFramebuffers(1, &capture_fbo);
  glDeleteRenderbuffers(1, &capture_rbo);
  
  PF("[assetm] loaded cubemap '%s''\n", path);

  cubemap_t c;
  c.environment = cubemap;
  c.irradiance  = irradiance_map;
  c.prefilter   = prefilter_map;
  // c.brdf_lut    = brdf_lut;
  return c;

}

// meshes -----------------------------------------------------------------------------------------

mesh_t* assetm_get_mesh_by_idx(int idx)
{
  assert(idx >= 0 && idx < mesh_data_len);
  return &mesh_data[idx];
}
int assetm_get_mesh_idx_dbg(const char* name, const char* file, const int line)
{  
  if (shget(mesh_idxs, name) <= 0)
  {
    assetm_create_mesh_dbg(name, file, line);
  }
  return shget(mesh_idxs, name);
}
mesh_t* assetm_get_mesh_dbg(const char* name, const char* file, const int line)
{
  if (shget(mesh_idxs, name) <= 0)
  {
    assetm_create_mesh_dbg(name, file, line);
  }
  return &mesh_data[shget(mesh_idxs, name)];
}
void assetm_create_mesh_dbg(const char* name, const char* file, const int line)
{
  // copy name and path as passed name might be deleted
  char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
  assert(name_cpy != NULL);
  strcpy(name_cpy, name);
  
  // load mesh -----------------------------------------------

  void*  buf = NULL;
  size_t buf_len = 0;

#ifdef ASSETM_NO_ZIP
  char path[64];
  int len = 0;
  sprintf(path, ASSET_PATH"meshes/%s", name);
  buf = (void*)file_read_len(path, &len);
  buf_len = len;
#else
  zip_entry_open(zip_meshes, name);
  {
    zip_entry_read(zip_meshes, &buf, &buf_len);
  }
  zip_entry_close(zip_meshes);
#endif
  ERR_CHECK(buf != NULL || buf_len != 0, "mesh '%s' requested in assetm_create_mesh() / assetm_get_mesh(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);

  mesh_t mesh = mesh_load_from_memory(buf, buf_len, name);

  free(buf);

  // ---------------------------------------------------------

  printf("[assetm] loaded mesh '%s'\n", name);

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  shput(mesh_idxs, name_cpy, mesh_data_len);
  arrput(mesh_data, mesh);
  mesh_data_len++;
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
  if (hmget(shader_idxs, type) <= 0)
  {
    assetm_create_shader_dbg(type, file, line);
  }
  return hmget(shader_idxs, type);
}
shader_t* assetm_get_shader_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  if (hmget(shader_idxs, type) <= 0)
  {
    assetm_create_shader_dbg(type, file, line);
  }
  return &shader_data[hmget(shader_idxs, type)];
}
void assetm_create_shader_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  shader_t s = assetm_create_shader_from_template_dbg(type, file, line);

  PF("[assetm] loaded shader '%d'\n", type);

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  hmput(shader_idxs, type, shader_data_len);
  arrput(shader_data, s);
  shader_data_len++;
}
shader_t assetm_create_shader_from_template_dbg(shader_template_type type, const char* file, const int line)
{
  ERR_CHECK(type != SHADER_TEMPLATE_NONE, "not a valid shader template\n -> [FILE] '%s', [LINE] %d", file, line);
  // void*  buf = NULL;
  // size_t buf_len = 0;

#ifdef ASSETM_NO_ZIP
  // char path[128]; 
  // sprintf(path, SHADERS_PATH"%s", name);
  // buf = file_read_len(path, (int*)&buf_len);
  const shader_template_t* s = shader_template_get(type);

  char vert_path[128]; 
  sprintf(vert_path, SHADERS_PATH"%s", s->vert);
  char frag_path[128]; 
  sprintf(frag_path, SHADERS_PATH"%s", s->frag);

  return shader_create_from_file(vert_path, frag_path, s->set_uniforms, s->name);
	
#else
  ERR("zip loading for shaders not yet implemented\n -> [FILE] '%s', [LINE] %d\n", file, line);
  // zip_entry_open(zip_materials, name);
  // {
  //   zip_entry_read(zip_materials, &buf, &buf_len);
  // }
  // zip_entry_close(zip_materials);
#endif 
  // ERR_CHECK(buf != NULL || buf_len != 0, "material '%s' requested in assetm_create_material() / assetm_get_material(), doesn't exist in the zip archive or folder.", name);
  // free(buf);
  // printf("[assetm] loaded shader '%s'\n", name);
}

// materials --------------------------------------------------------------------------------------

// @TODO: use '_dbg' func's and macros for __FILE__ & __LINE__ as well 

material_t* assetm_get_material_by_idx(int idx)
{
  assert(idx >= 0 && idx < material_data_len);
  return &material_data[idx];
}
int assetm_get_material_idx(material_template_type type)
{  
  if (hmget(material_idxs, type) <= 0)
  {
    assetm_create_material(type);
  }
  return hmget(material_idxs, type);
}
material_t* assetm_get_material(material_template_type type)
{
  if (hmget(material_idxs, type) <= 0)
  {
    assetm_create_material(type);
  }
  return &material_data[hmget(material_idxs, type)];
}
void assetm_create_material(material_template_type type)
{
  const material_template_t* m = material_template_get(type);
  material_t mat = material_load_from_template(m);

  // ---------------------------------------------------------

  printf("[assetm] loaded material '%d'\n", type);

  // put texture index in tex array into the value of the hashmap with the texture name as key 
  // and put the created texture into the tex array
  hmput(material_idxs, type, material_data_len);
  arrput(material_data, mat);
  material_data_len++;
}


// @NOTE: old code for loading 6 seperate images as a cubemap
// u32 assetm_create_cubemap_dbg(const char* name, const char* ext, const char* file, const int line)
// {
//   u32 cube_map;
//   glGenTextures(1, &cube_map);
//   glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
// 
//   char cube_map_paths[6][32];
//   sprintf(cube_map_paths[0], "%s/right%s", name, ext);
//   sprintf(cube_map_paths[1], "%s/left%s", name, ext);
//   sprintf(cube_map_paths[2], "%s/bottom%s", name, ext);
//   sprintf(cube_map_paths[3], "%s/top%s", name, ext);
//   sprintf(cube_map_paths[4], "%s/front%s", name, ext);
//   sprintf(cube_map_paths[5], "%s/back%s", name, ext);
// 
//   void*  buf = NULL;
//   size_t buf_len = 0;
// 
//   int width, height, channel_num;
//   for (u32 i = 0; i < 6; ++i)
//   {
// #ifdef ASSETM_NO_ZIP
//     char path[64];
//     int len = 0;
//     sprintf(path, ASSET_PATH"textures/%s", name);
//     buf = (void*)file_read_len(path, &len);
//     buf_len = len;
//     ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in assetm_load_cubemap_hdr(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", path, file, line);
// #else 
//     zip_entry_open(zip_textures, cube_map_paths[i]);
//     {
//       zip_entry_read(zip_textures, &buf, &buf_len);
//     }
//     zip_entry_close(zip_textures);
//     ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in assetm_create_cubemap(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", name, file, line);
// #endif
// 
//     stbi_set_flip_vertically_on_load(true);
//     u8* data = stbi_load_from_memory(buf, buf_len, &width, &height, &channel_num, STBI_rgb_alpha); // 0
// 
//     int gl_internal_format = 0;
//     int gl_format          = 0;
//     switch (channel_num)
//     {
//         case 1:
//             gl_internal_format = GL_SRGB8; // GL_R8
//             gl_format = GL_RED;
//             break;
//         case 3:
//             gl_internal_format = GL_SRGB; // gl_RGB
//             gl_format = GL_RGB;
//             break;
//         case 4:
//             gl_internal_format = GL_SRGB_ALPHA; // GL_RGBA
//             gl_format = GL_RGBA;
//             break;
//     }
//     assert(gl_format != 0);
//     if (data)
//     {
//       // we can increment the first param like this because the enum linearly increments
//       glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, data );
//       stbi_image_free(data);
//     }
//     else
//     {
//       ERR("cubemap tex failed to load at path: %s\n", cube_map_paths[i]);
//       stbi_image_free(data);
//     }
//   }
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// 
//   return cube_map;
// }


