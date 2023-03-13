#ifndef ASSET_IO_H
#define ASSET_IO_H

#include "global/global.h"
#include "core/types/types.h"

// --- mesh format ---
// only save verts, different format for animated meshes later
// 3:pos + 3:norm + 2:uv + 3:tan  =  11: f32, 44: bytes
//
// u32: indices length
// ->
//    * u32 per indices
// u32: vert length
// ->
// 11 * f32 per vert

// --- texture format ---
// u32: width
// u32: height
// u32: channels
// u8[]: pixels, len is w * h * channels

// @DOC: enum for all the asset types
//       cur only used in assset_io_serialize_archive
typedef enum asset_type
{
  ASSET_TYPE_TEXTURE,
  ASSET_TYPE_MESH,
  ASSET_TYPE_MATERIAL,
  ASSET_TYPE_SHADER
}asset_type;

// @DOC: initialize asset_io call this before any other calls to asset_io
void asset_io_init();
// @DOC: convert .fbx to .mesh
//       ! name gets added .fbx and .mesh endings automatically
//       name: name without file ending i.e. 'demon02'
void asset_io_convert_mesh(const char* name);
// @DOC: put verts & indices into byte buffer
//       buffer:      stb_ds u8 array, needs to be freed using arrfree(), needs to be NULL when passed into func
//       verts:       stb_ds or regular array
//       verts_len:   length of verts
//       indices:     stb_ds or regular array
//       indices_len: length of indices
void asset_io_serialize_mesh(u8** buffer, f32* verts, u32 verts_len, u32* indices, u32 indices_len);
// @DOC: loads mesh from '.mesh' file
//       name: file name, without '.mesh'
mesh_t asset_io_load_mesh(const char* name);
// @DOC: extract verts & indices from byte buffer
//       buffer:  stb_ds u8 array, needs to be freed using arrfree(), needs to be NULL when passed into func
//       verts:   stb_ds u8 array, needs to be freed using arrfree(), needs to be NULL when passed into func
//       indices: stb_ds u8 array, needs to be freed using arrfree(), needs to be NULL when passed into func
void asset_io_deserialize_mesh(u8* buffer, f32** verts, u32** indices);

// @DOC: convert .png/.jpg/etc. to .tex
//       name: name with file ending i.e. 'albedo.png'
void asset_io_convert_texture_dbg(const char* name, const char* _file, const int _line);
#define asset_io_convert_texture(name) asset_io_convert_texture_dbg((name), __FILE__, __LINE__)
// @DOC: put header & pixels into byte buffer
//       ! returned u8 buffer need to be freed
//       pixels:     the loaded pixel data
//       w:          width of texture
//       h:          height of texture
//       channels:   r = 1, rgb = 3, rgba = 4
//       buffer_len: gets set to the total returned buffer length
//       returns:    u8 array of serialized data, needs to be freed
u8* asset_io_serialize_texture(u8* pixels, u32 w, u32 h, u32 channels, u32* buffer_len);

u32 asset_io_load_texture_handle(const char* name, bool srgb);
// @DOC: load a .tex texture
//       name: name of texture including file ending, i.e. 'albedo.png'
//       srgb: whether to load the texture as srgb, for hdr
texture_t asset_io_load_texture(const char* name, bool srgb);
texture_t asset_io_load_texture_full_path(const char* path, bool srgb);
texture_t asset_io_load_texture_full_path_formatted(const char* path, bool srgb, u32 target_channels);

  // @DOC: extract header & pixels from .tex file laoded into u8 buffer
//       buffer:   the loaded .tex file
//       pixels:   gets set to point to the pixel data, in the returned u8* buffer, len = w * h * channels
//       w:        gets set to width of texture
//       h:        gets set to height of texture
//       w:        gets set to width of texture
//       channels: r = 1, rgb = 3, rgba = 4
void asset_io_deserialize_texture(u8* buffer, u8** pixels, u32* w, u32* h, u32* channels);
// pixels also need to be freed
void asset_io_deserialize_texture_formatted(u8* buffer, u32 target_channels, u8** pixels, u32* w, u32* h, u32* channels);

// @NOTE: not done, dosnt have a header, just bunch of tex files one after another
void asset_io_serialize_archive(const char* dir_path, int initial_dir_path_len, u8** rtn, u32* rtn_size, asset_type type);

u8* asset_io_texture_write_pixels_to_buffer_u8(texture_t* t,  u32 gl_type, u32* buffer_len);
// f32* asset_io_texture_write_pixels_to_buffer_f32(texture_t* t,  u32 gl_type, u32* buffer_len);
void asset_io_texture_write_pixels_to_file(texture_t* t,  u32 gl_type, const char* path);

#endif
