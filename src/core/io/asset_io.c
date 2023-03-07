#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "core/types/mesh.h"
#include "core/core_data.h"
#include "serialization/serialization.h"

static core_data_t* core_data = NULL;

void asset_io_convert_mesh(const char* file_src, const char* file_dest)
{
  core_data = core_data_get();

  // -- load fbx file into memory --
  void*  buf = NULL;
  size_t buf_len = 0;

// #ifdef ASSETM_NO_ZIP
  char path[ASSET_PATH_MAX +64];
  int len = 0;
  sprintf(path, "%smeshes/%s", core_data->asset_path, name);
  buf = (void*)file_read_len(path, &len);
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
  mesh_load_data_from_memory(buf, buf_len, name, &verts, &indices);
  free(buf);
  
  // -- save into file_dest as binary --

  u8* buffer = NULL;
  asset_io_save_mesh(&buffer, verts, arrlen(verts), indices, arrlen(indices));

  
  file_io_write(file_dest, const char* txt, int len)

  arrfree(buffer);
  arrfree(verts);
  arrfree(indices);
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
