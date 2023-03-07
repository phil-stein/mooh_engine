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


void asset_io_convert_mesh(const char* file_src, const char* file_dest);
void asset_io_save_mesh();
mesh_t asset_io_load_mesh(const char* path);


#endif
