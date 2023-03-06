#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "../../core/types/types.h"

void cubemap_free();
cubemap_t cubemap_load_dbg(const char* path, const char* file, const int line); 
// @DOC: load cubemap from .hdr file
//       path: path to .hdr file
#define cubemap_load(path)       cubemap_load_dbg(path, __FILE__, __LINE__)

// -- old --

// u32 assetm_create_cubemap_dbg(const char* name, const char* ext, const char* file, const int line);
// @NOTE: no longer in use
// #define assetm_create_cubemap(name, ext)    assetm_create_cubemap_dbg(name, ext, __FILE__, __LINE__)



#endif
