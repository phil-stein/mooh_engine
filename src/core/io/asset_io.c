#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "core/io/file_io.h"
#include "core/types/mesh.h"
#include "core/core_data.h"
#include "core/debug/debug_timer.h"
#include "core/debug/debug_opengl.h"
#include "serialization/serialization.h"

#include "stb/stb_ds.h"
#include "stb/stb_image.h"

#include <direct.h>
#include "dirent/dirent.h"

#define ASSET_IO_NAME_MAX 64
char name_src[ASSET_IO_NAME_MAX];
char name_dest[ASSET_IO_NAME_MAX];

static core_data_t* core_data = NULL;


void asset_io_init()
{
  core_data = core_data_get();
 
  // // @TMP: test archive
  // u8* buffer = NULL;
  // u32 buffer_len = 0;
  // TIMER_START("archive");
  // asset_io_serialize_archive(core_data->asset_path, strlen(core_data->asset_path), &buffer, &buffer_len, ASSET_TYPE_TEXTURE);
  // TIMER_STOP_PRINT();

  // // @TODO: make global path string
  // char path[ASSET_PATH_MAX + 64];
  // sprintf(path, "%s/%s", core_data->asset_path, "textures.pack");
  // P_STR(path);
  // file_io_write_bytes(path, buffer, buffer_len);
  // free(buffer);
}

// -- mesh --

void asset_io_convert_mesh(const char* name)
{
  // -- load fbx file into memory --
  void*  buf = NULL;
  size_t buf_len = 0;

  ERR_CHECK(strlen(name) < ASSET_IO_NAME_MAX - 5, "name for mesh too long");
  sprintf(name_src,  "%s.fbx",  name);
  sprintf(name_dest, "%s.mesh", name);

// #ifdef ASSETM_NO_ZIP
  char path[ASSET_PATH_MAX +7 +ASSET_IO_NAME_MAX];
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
  FREE(buf);
  
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
  // PF("[mesh] | %s |\n", name);
  // TIMER_START_COUNTER("read mesh file    |");
  int length = 0;
  char path[ASSET_PATH_MAX +64];
  sprintf(path, "%smeshes/%s%s", core_data->asset_path, name, ".mesh");
  u8* buffer = file_io_read_bytes(path, &length);
  // TIMER_STOP();

    
  // TIMER_START_COUNTER("make mesh         |");
  f32* verts   = NULL;
  u32* indices = NULL;
  asset_io_deserialize_mesh(buffer, &verts, &indices);
  
  mesh_t mesh;
  mesh_make_indexed(verts, arrlen(verts), indices, arrlen(indices), &mesh);
  arrfree(verts);
  arrfree(indices);
  // TIMER_STOP();

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

// -- textures --

void asset_io_convert_texture_dbg(const char* name, const char* _file, const int _line)
{
  // -- load file --
  void*  buf = NULL;
  size_t buf_len = 0;
  
  // TIMER_START(" -> load tex file");
  char path[ASSET_PATH_MAX + ASSET_IO_NAME_MAX + 12];
  int len = 0;
  sprintf(path, "%stextures/%s", core_data->asset_path, name);
  buf = (void*)file_io_read_len(path, &len);
  buf_len = len;
  ERR_CHECK(buf != NULL || buf_len != 0, "texture '%s' requested in asset_io_convert_texture(), doesn't exist in the asset folder.\n -> [FILE] '%s', [LINE] %d", name, _file, _line);

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
  FREE(buf);
  FREE(buffer);
}
u8* asset_io_serialize_texture(u8* pixels, u32 w, u32 h, u32 channels, u32* buffer_len)
{
  const int header_size = (sizeof(u32) * 3);  // w, h, channels
  const int pixels_len  = w * h * channels * sizeof(u8);
  *buffer_len = pixels_len + header_size;
  u8* buffer;
  MALLOC(buffer, *buffer_len);
  
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

u32 asset_io_load_texture_handle(const char* name, bool srgb)
{
  return asset_io_load_texture(name, srgb).handle;
}
texture_t asset_io_load_texture(const char* name, bool srgb)
{
  // copy name into name_dest without file ending '.png' etc.
  u32 i = 0; while(name[i] != '.' && name[i +1] != '\0') { name_dest[i] = name[i]; i++; } name_dest[i] = '\0';
  char path[ASSET_PATH_MAX + ASSET_IO_NAME_MAX + 12];
  sprintf(path, "%stextures/%s%s", core_data->asset_path, name_dest, ".tex");
  
  // PF("[asset_io] loaded \"%s\"\n", name);
  
  return asset_io_load_texture_full_path(path, srgb);
}
texture_t asset_io_load_texture_full_path(const char* path, bool srgb)
{
  // PF("[tex] | %s |\n", name);
  // TIMER_START_COUNTER("read texture file |");

  int length = 0;
  u8* buffer = file_io_read_bytes(path, &length);
  // TIMER_STOP();

  // TIMER_START_COUNTER("make texture      |");
  u8* pixels;
  u32 w, h, channels;
  asset_io_deserialize_texture(buffer, &pixels, &w, &h, &channels);

  u32 handle = texture_create_from_pixels(pixels, (size_t)w, (size_t)h, (int)channels, srgb); 
  texture_t t;
  t.loaded = true;
  t.handle = handle;
  t.width = w;
  t.height = h;
  t.channel_nr = channels;

  FREE(buffer);
  // pixels is part of buffer
  // TIMER_STOP();
 
  return t;
}
texture_t asset_io_load_texture_full_path_formatted(const char* path, bool srgb, u32 target_channels)
{
  // PF("[tex] | %s |\n", path);
  // TIMER_START_COUNTER("read texture file |");

  int length = 0;
  u8* buffer = file_io_read_bytes(path, &length);
  // TIMER_STOP();

  // TIMER_START_COUNTER("make texture      |");
  u8* pixels;
  u32 w, h, channels;
  asset_io_deserialize_texture_formatted(buffer, target_channels, &pixels, &w, &h, &channels);

  u32 handle = texture_create_from_pixels(pixels, (size_t)w, (size_t)h, (int)target_channels, srgb); 
  texture_t t;
  t.loaded = true;
  t.handle = handle;
  t.width = w;
  t.height = h;
  t.channel_nr = target_channels;

  FREE(buffer);
  FREE(pixels);
  // TIMER_STOP();
 
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
void asset_io_deserialize_texture_formatted(u8* buffer, u32 target_channels, u8** pixels, u32* w, u32* h, u32* channels)
{ 
  // deserialize u8 header into u32
  u8* data = buffer;
  // w
  *w  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;
  // h
  *h  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;
  // channels
  *channels  = ((u32)data[3]) + ((u32)data[2] << 8) + ((u32)data[1] << 16) + ((u32)data[0] << 24);
  data += 4;

  // P_U32(*(channels));
  // P_U32(target_channels);
  
  u32 data_len   = (*w) * (*h) * (*channels);
  u32 pixels_len = (*w) * (*h) *  target_channels; 
  // P_U32( data_len);
  // P_U32(pixels_len);

  MALLOC((*pixels), pixels_len * sizeof(u8));
  u8* _pixels = (*pixels); 
  
  u32 d_pos = 0;
  u32 p_pos = 0;
  const u32 data_step      = MIN(target_channels, (*channels));
  const u32 data_step_diff = MAX(0, (int)(*channels) - (int)target_channels);
  const u32 pixels_step = MAX(0, (int)target_channels - (int)(*channels));
  // P_U32(data_step);
  // P_U32(data_step_diff);
  // P_INT(pixels_step);
  
  for (u32 i = 0; i < data_len && i < pixels_len; i += data_step) 
  {
    // copy pixels
    for (u32 j = 0; j < data_step; ++j) 
    {
      ERR_CHECK(p_pos < pixels_len, "[0] pixels_len: %d | p_pos: %d | i: %d |", pixels_len, p_pos, i);
      ERR_CHECK(d_pos < data_len,   "[0] data_len: %d | d_pos: %d | i: %d | j: %d |", data_len, d_pos, i, j);
      _pixels[p_pos++] = data[d_pos++];
    }
    d_pos += data_step_diff; // in case channels > than target_channels
    // fill/skip rest
    for (u32 j = 0; j < pixels_step; ++j) 
    {
      ERR_CHECK(p_pos < pixels_len, "[1] pixels_len: %d | p_pos: %d | i: %d | j: %d |", pixels_len, p_pos, i, j);
      _pixels[p_pos++] = (*channels) == 1 ? data[d_pos -1] : 255;
    }
  }
}

void asset_io_serialize_archive(const char* dir_path, int initial_dir_path_len, u8** rtn, u32* rtn_size, asset_type type)
{
	char path[256];
	struct dirent* dp;
	DIR* dir = opendir(dir_path);
	// unable to open directory stream
	if (!dir) { return; }

	// recursively read the directory and its sub-directories
	while ((dp = readdir(dir)) != NULL)
	{
		// check that the file currently read isn't a directory
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			// printf("%s, \t[-1]%c, [-2]%c, [-3]%c, [-4]%c\n", dp->d_name, dp->d_name[dp->d_namlen - 1], dp->d_name[dp->d_namlen - 2], dp->d_name[dp->d_namlen - 3], dp->d_name[dp->d_namlen - 4]);
	
			// construct new path from our base path
			strcpy(path, dir_path);
			strcat(path, "\\");
			strcat(path, dp->d_name);		
      
      int d_name_len = strlen(dp->d_name);
      // char* _name = path + initial_dir_path_len +1; // initial_... to cut off /assets/, +1 to cut off / 

      // @UNSURE: use as way to pack albedo, metallic, normal, roughness into one file

      if (type == ASSET_TYPE_TEXTURE        &&
          dp->d_name[d_name_len - 4] == '.' &&
		      dp->d_name[d_name_len - 3] == 't' &&
		      dp->d_name[d_name_len - 2] == 'e' &&
		      dp->d_name[d_name_len - 1] == 'x' )
      {
        PF("[.tex] \"%s\"\n -> \"%s\"\n", dp->d_name, path);
        int length = 0;
        u8* buffer = file_io_read_bytes(path, &length);
        *rtn = realloc(*rtn, (*rtn_size + length) * sizeof(u8));
        memcpy(*rtn + *rtn_size, buffer, length);
        *rtn_size += length;
      }
      else if (type == ASSET_TYPE_MESH           &&
               dp->d_name[d_name_len - 5] == '.' &&
		           dp->d_name[d_name_len - 4] == 'm' &&
		           dp->d_name[d_name_len - 3] == 'e' &&
		           dp->d_name[d_name_len - 2] == 's' &&
		           dp->d_name[d_name_len - 1] == 'h' )
      {
        // PF("[.mesh] \"%s\"\n -> \"%s\"\n", dp->d_name, _name);
      }


			asset_io_serialize_archive(path, initial_dir_path_len, rtn, rtn_size, type); // search recursively
		}
	}

	// close the stream
	closedir(dir);
  
}

u8* asset_io_texture_write_pixels_to_buffer_u8(texture_t* t,  u32 gl_type, u32* buffer_len)
{
  *buffer_len = t->width * t->height * t->channel_nr * sizeof(u8);
  u8* buffer;
  MALLOC(buffer, *buffer_len);
  _glActiveTexture(GL_TEXTURE0);
  _glBindTexture(GL_TEXTURE_2D, t->handle);
  _glGetTexImage(GL_TEXTURE_2D, 0, gl_type, GL_UNSIGNED_BYTE, buffer);

  return buffer;
}
// f32* asset_io_texture_write_pixels_to_buffer_f32(texture_t* t,  u32 gl_type, u32* buffer_len)
// {
//   *buffer_len = t->width * t->height * t->channel_nr * sizeof(f32);
//   f32* buffer = malloc(*buffer_len);
//   _glActiveTexture(GL_TEXTURE0);
//   _glBindTexture(GL_TEXTURE_2D, t->handle);
//   _glGetTexImage(GL_TEXTURE_2D, 0, gl_type, GL_FLOAT, buffer);
// 
//   return buffer;
// }
void asset_io_texture_write_pixels_to_file(texture_t* t,  u32 gl_type, const char* path)
{
  // u32 handle, u32 width, u32 height, u32 channel_nr, u32 gl_type,

  u32 pixels_len = 0;
  u8* pixels = asset_io_texture_write_pixels_to_buffer_u8(t, gl_type, &pixels_len);
 
  u32 buffer_len = 0;
  u8* buffer = asset_io_serialize_texture(pixels, t->width, t->height, t->channel_nr, &buffer_len);
  
  file_io_write_bytes(path, buffer, buffer_len);

  FREE(pixels);
}

