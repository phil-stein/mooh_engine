#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "core/io/file_io.h"
#include "core/types/mesh.h"
#include "core/core_data.h"
#include "serialization/serialization.h"

#include "stb/stb_ds.h"

static core_data_t* core_data = NULL;

void asset_io_init()
{
  core_data = core_data_get();
}

void asset_io_convert_mesh(const char* name_src, const char* name_dest)
{
  // -- load fbx file into memory --
  void*  buf = NULL;
  size_t buf_len = 0;

// #ifdef ASSETM_NO_ZIP
  char path[ASSET_PATH_MAX +64];
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
   
  len = 0;
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
  sprintf(path, "%smeshes/%s", core_data->asset_path, name);
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

