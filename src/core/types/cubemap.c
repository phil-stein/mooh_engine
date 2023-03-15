#include "core/types/cubemap.h"
#include "core/io/assetm.h"
#include "core/core_data.h"
#include "core/io/file_io.h"
#include "core/debug/debug_opengl.h"
#include "math/math_inc.h"

#include "GLAD/glad.h"
#include "stb/stb_ds.h"
#include "stb/stb_image.h"
#include "zip/zip.h"

static core_data_t* core_data = NULL;

  
// @BUGG: @OPTIMIZATION: feeing cubemap doesnt seem to free memory 
void cubemap_free()
{
  core_data = core_data_get();

  if (!core_data->cube_map.loaded) { return; }
  texture_free_handle(core_data->cube_map.environment);
  texture_free_handle(core_data->cube_map.irradiance);
  texture_free_handle(core_data->cube_map.prefilter);
  core_data->cube_map.loaded = false;
}

cubemap_t cubemap_load_dbg(const char* path, const char* _file, const int _line) 
{
  core_data = core_data_get();
 
  // @BUGG: @OPTIMIZATION: reloading cubemap adds a lot of memory
  if (core_data->cube_map.loaded) { cubemap_free(); }

  // fix seams betweencubemap faces
  // this shoud be activated in renderer
  _glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  
  // load hdr image ----------------------------------------------------------------------
 
  void*  buf = NULL;
  size_t buf_len = 0;
// #ifdef ASSETM_NO_ZIP
  char _path[ASSET_PATH_MAX +64];
  int len = 0;
  SPRINTF(ASSET_PATH_MAX + 64, _path, "%stextures/%s", core_data->asset_path, path);
  buf = (void*)file_io_read_len(_path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in cubemap_load(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", path, _file, _line);
// #else 
//   zip_entry_open(zip_textures, path);
//   {
//     zip_entry_read(zip_textures, &buf, &buf_len);
//   }
//   zip_entry_close(zip_textures);
//   ERR_CHECK(buf != NULL || buf_len != 0, "cubemap_hdr '%s' requested in cubemap_load(), doesn't exist in the zip archive.\n -> [FILE] '%s', [LINE] %d", path, file, line);
// #endif

  stbi_set_flip_vertically_on_load(false);
  int width, height, channels;
  float *data = stbi_loadf_from_memory(buf, buf_len, &width, &height, &channels, 0);
  u32 hdr_texture;
  if (data)
  {
    _glGenTextures(1, &hdr_texture);
    _glBindTexture(GL_TEXTURE_2D, hdr_texture);
    _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 

    _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  { ERR("failed to load HDR image."); }

  // gen framebuffer ---------------------------------------------------------------------
    
  u32 capture_fbo, capture_rbo;
  _glGenFramebuffers(1, &capture_fbo);
  _glGenRenderbuffers(1, &capture_rbo);

  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  _glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  _glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  _glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_fbo);

  // gen cubemap -------------------------------------------------------------------------
 
  // @TODO: clear cubema ???

  u32 cubemap;
  _glGenTextures(1, &cubemap);
  _glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  for (u32 i = 0; i < 6; ++i)
  {
    // note that we store each face with 16 bit floating point values
    _glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
        512, 512, 0, GL_RGB, GL_FLOAT, NULL);
  }
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // render cubemap ----------------------------------------------------------------------
 
  mesh_t* cube_mesh = assetm_get_mesh("cube");

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
  _glActiveTexture(GL_TEXTURE0);
  _glBindTexture(GL_TEXTURE_2D, hdr_texture);

  _glDisable(GL_CULL_FACE);
  _glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  for (u32 i = 0; i < 6; ++i)
  {
    shader_set_mat4(&core_data->equirect_shader, "view", view_mats[i]);
    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _glBindVertexArray(cube_mesh->vao);
    if (cube_mesh->indexed)
    { _glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
    else
    { _glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
  }
  _glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  
  // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
  _glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  _glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  
  // render irradiencemap ----------------------------------------------------------------

  u32 irradiance_map;
  _glGenTextures(1, &irradiance_map); 
  _glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
  for (u32 i = 0; i < 6; ++i)
  {
    _glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, 
        GL_RGB, GL_FLOAT, NULL);
  }
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  _glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  _glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);  

  shader_use(&core_data->irradiance_map_shader);
  shader_set_mat4(&core_data->irradiance_map_shader, "proj", proj);
  shader_set_int(&core_data->irradiance_map_shader, "environment_map", 0);
  _glActiveTexture(GL_TEXTURE0);
  _glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

  _glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  for (u32 i = 0; i < 6; ++i)
  {
    shader_set_mat4(&core_data->irradiance_map_shader, "view", view_mats[i]);
    _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map, 0);
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _glBindVertexArray(cube_mesh->vao);
    if (cube_mesh->indexed)
    { _glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
    else
    { _glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
  }
  _glBindFramebuffer(GL_FRAMEBUFFER, 0);

  
  // gen radiance map --------------------------------------------------------------------
  
  u32 prefilter_map;
  _glGenTextures(1, &prefilter_map); 
  _glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map);
  for (u32 i = 0; i < 6; ++i)
  {
    _glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
  }
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
  _glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  _glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // render radiance map -----------------------------------------------------------------
  
  shader_use(&core_data->prefilter_shader);
  shader_set_mat4(&core_data->prefilter_shader, "proj", proj);
  _glActiveTexture(GL_TEXTURE0);
  _glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  shader_set_int(&core_data->prefilter_shader, "environment_map", 0);

  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  u32 max_mip_levels = 5;
  for (u32 mip = 0; mip < max_mip_levels; ++mip)
  {
    // reisze framebuffer according to mip-level size.
    u32 mip_w = 128 * pow(0.5, mip);
    u32 mip_h = 128 * pow(0.5, mip);
    _glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    _glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_w, mip_h);
    _glViewport(0, 0, mip_w, mip_h);

    float roughness = (float)mip / (float)(max_mip_levels - 1);
    shader_set_float(&core_data->prefilter_shader, "roughness", roughness);
    for (u32  i = 0; i < 6; ++i)
    {
      shader_set_mat4(&core_data->prefilter_shader, "view", view_mats[i]);
      _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map, mip);

      _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      _glBindVertexArray(cube_mesh->vao);
      if (cube_mesh->indexed)
      { _glDrawElements(GL_TRIANGLES, cube_mesh->indices_count, GL_UNSIGNED_INT, 0); }
      else
      { _glDrawArrays(GL_TRIANGLES, 0, cube_mesh->verts_count); }
    }
  }
  _glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _glEnable(GL_CULL_FACE);
  
  _glDeleteFramebuffers(1, &capture_fbo);
  _glDeleteRenderbuffers(1, &capture_rbo);

  // texture_free_handle(prefilter_map);
  
  ASSETM_PF("[cubemap] loaded cubemap '%s''\n", path);

  cubemap_t c;
  c.loaded = true;
  ERR_CHECK(strlen(path) < CUBE_MAP_NAME_MAX, "not enough space in cube_map.name for the given name / path. %d|%d", (int)strlen(path), CUBE_MAP_NAME_MAX); 
  strcpy(c.name, path);
  c.environment = cubemap;
  c.irradiance  = irradiance_map;
  c.prefilter   = prefilter_map;
  // c.brdf_lut    = brdf_lut;
  c.intensity   = 1.0f;

  texture_free_handle(hdr_texture);

  return c;

}

// -- old --

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
//     sprintf(path, core_data->asset_path"textures/%s", name);
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


