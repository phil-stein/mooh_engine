#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "core/io/file_io.h"
#include "core/types/mesh.h"
#include "core/core_data.h"
#include "serialization/serialization.h"

#include "stb/stb_ds.h"
#include "stb/stb_image.h"

#define NAME_MAX 64
char name_src[NAME_MAX];
char name_dest[NAME_MAX];

static core_data_t* core_data = NULL;


void asset_io_init()
{
  core_data = core_data_get();
}

void asset_io_convert_mesh(const char* name)
{
  // -- load fbx file into memory --
  void*  buf = NULL;
  size_t buf_len = 0;

  ERR_CHECK(strlen(name) < NAME_MAX - 5, "name for mesh too long");
  sprintf(name_src,  "%s.fbx",  name);
  sprintf(name_dest, "%s.mesh", name);

// #ifdef ASSETM_NO_ZIP
  char path[ASSET_PATH_MAX +7 +NAME_MAX];
  int len = 0;
  sprintf(path, "%smeshes/%s", core_data->asset_path, name_src);
  buf = (void*)file_io_read_len(path, &len);
  buf_len = len;
// #else
//   zip_entry_open(zip_meshes, name);
//   {
//     zip_entry_read(zip_meshes, &buf, &buf_len);
//   }
//   zip_entry_close(zip_meshes);
// #endif

  // -- extract verts and indices from fbx file --

  f32* verts   = NULL;
  u32* indices = NULL;
  mesh_load_data_from_memory(buf, buf_len, name_src, &verts, &indices);
  free(buf);
  
  // -- save into file_dest as binary --

  u8* buffer = NULL;
  asset_io_serialize_mesh(&buffer, verts, arrlen(verts), indices, arrlen(indices));
  arrfree(verts);
  arrfree(indices);
   
  sprintf(path, "%smeshes/%s", core_data->asset_path, name_dest); 
  file_io_write_bytes(path, buffer, arrlen(buffer));
  arrfree(buffer);
}

void asset_io_serialize_mesh(u8** buffer, f32* verts, u32 verts_len, u32* indices, u32 indices_len)
{ 
  serialization_serialize_u32(buffer, indices_len);

  for (u32 i = 0; i < indices_len; ++i)
  {
    serialization_serialize_u32(buffer, indices[i]);
  }

  serialization_serialize_u32(buffer, verts_len);

  for (u32 i = 0; i < verts_len; ++i)
  {
    serialization_serialize_f32(buffer, verts[i]);
  }
}

mesh_t asset_io_load_mesh(const char* name)
{
  int length = 0;
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%smeshes/%s%s", core_data->asset_path, name, ".mesh");
  u8* buffer = file_io_read_bytes(path, &length);

    
  f32* verts   = NULL;
  u32* indices = NULL;

  asset_io_deserialize_mesh(buffer, &verts, &indices);
  
  mesh_t mesh;
  mesh_make_indexed(verts, arrlen(verts), indices, arrlen(indices), &mesh);
  arrfree(verts);
  arrfree(indices);

  return mesh;
}

void asset_io_deserialize_mesh(u8* buffer, f32** verts, u32** indices)
{
  u32 offset = 0;

  u32 indices_len = serialization_deserialize_u32(buffer, &offset);

  for (u32 i = 0; i < indices_len; ++i)
  {
    arrput(*indices, serialization_deserialize_u32(buffer, &offset));
  }

  u32 verts_len = serialization_deserialize_u32(buffer, &offset);

  for (u32 i = 0; i < verts_len; ++i)
  {
    arrput(*verts, serialization_deserialize_f32(buffer, &offset));
  }
}

void asset_io_convert_texture_dbg(const char* name, const char* file, const int line)
{
  // -- load file --
  void*  buf = NULL;
  size_t buf_len = 0;
  
  // TIMER_START(" -> load tex file");
  char path[ASSET_PATH_MAX + NAME_MAX + 12];
  int len = 0;
  sprintf(path, "%stextures/%s", core_data->asset_path, name);
  buf = (void*)file_io_read_len(path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in asset_io_convert_texture(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", name, file, line);

  // -- load pixels --
  stbi_set_flip_vertically_on_load(true);
  int w, h, channels;
  u8* pixels = stbi_load_from_memory(buf, buf_len, &w, &h, &channels, 0);
    
  // for (u32 i = 0; i < w * h * channels; ++i)
  // {
  //   ERR_CHECK(pixels[i] == 255, "pixels[%d]: %u\n", i, pixels[i]);
  // }
  // -- convert --
  u32 buffer_len = 0;
  u8* buffer = asset_io_serialize_texture(pixels, w, h, channels, &buffer_len);
  
  // -- write to file --
  for (u32 i = 0; i < strlen(name) - 4; ++i) { name_dest[i] = name[i]; name_dest[i +1] = '\0'; }
  sprintf(path, "%stextures/%s.tex", core_data->asset_path, name_dest);  
  file_io_write_bytes(path, buffer, buffer_len);
  
  // -- cleanup --
  stbi_image_free(pixels);
  free(buf);
  free(buffer);
}
u8* asset_io_serialize_texture(u8* pixels, u32 w, u32 h, u32 channels, u32* buffer_len)
{
  const int header_size = (sizeof(u32) * 3);  // w, h, channels
  const int pixels_len  = w * h * channels * sizeof(u8);
  *buffer_len = pixels_len + header_size;
  u8* buffer = malloc(*buffer_len);
  
  // pack header into u8
  // w
  buffer[0] = w >> 24;
  buffer[1] = w >> 16;
  buffer[2] = w >>  8;
  buffer[3] = w; 
  // h
  buffer[4] = h >> 24;
  buffer[5] = h >> 16;
  buffer[6] = h >> 8;
  buffer[7] = h;
  // channels
  buffer[8]  = channels >> 24;
  buffer[9]  = channels >> 16;
  buffer[10] = channels >> 8;
  buffer[11] = channels;

  // copy pixels into buffer after header
  memcpy(buffer + header_size, pixels, pixels_len);

  return buffer;
}

texture_t asset_io_load_texture(const char* name, bool srgb)
{
  // copy name into name_dest without file ending '.png' etc.
  u32 i = 0; while(name[i] != '.' && name[i +1] != '\0') { name_dest[i] = name[i]; i++; } name_dest[i] = '\0';
  // P_STR(name_dest);
  int length = 0;
  char path[ASSET_PATH_MAX + NAME_MAX + 10];
  sprintf(path, "%stextures/%s%s", core_data->asset_path, name_dest, ".tex");
  // P_STR(path);
  u8* buffer = file_io_read_bytes(path, &length);

  u8* pixels;
  u32 w, h, channels;
  asset_io_deserialize_texture(buffer, &pixels, &w, &h, &channels);

  u32 handle = texture_create_from_pixels(pixels, (size_t)w, (size_t)h, (int)channels, srgb); 
  texture_t t;
  t.handle = handle;
  t.width = w;
  t.height = h;

  free(buffer);
  // pixels is part of buffer
  
  return t;
}

void asset_io_deserialize_texture(u8* buffer, u8** pixels, u32* w, u32* h, u32* channels)
{ 
  // deserialize u8 header into u32
  u8* data = buffer;
  // w
  *w  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;
  // P_U32(*w);
  // h
  *h  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;
  // P_U32(*h);
  // channels
  *channels  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;
  // P_U32(*channels);

  // const int header_size = (sizeof(u32) * 3);  // w, h, channels
  // data points to buffer + 12, which is eq to buffer + header_size
  *pixels = data; // buffer + header_size;
}

