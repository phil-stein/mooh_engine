#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "serialization/serialization.h"


void asset_io_convert_mesh(const char* file_src, const char* file_dest)
{
  // mesh_load_fbx
  // assert_io_save_mesh
}


void asset_io_save_mesh(f32* verts, u32 verts_len, u32* indices, u32 indices_len)
{
  u8* buffer = NULL;
  
  serialization_serialize_u32(buffer, indices_len);

  for (u32 i = 0; i < indices_len; ++i)
  {
    serialization_serialize_u32(buffer, indices[i]);
  }

  serialization_serialize_u32(buffer, verts_len);

  for (u32 i = 0; i < verts_len; ++i)
  {
  }
}
