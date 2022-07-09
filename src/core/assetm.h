#ifndef ASSETM_H
#define ASSETM_H

#include "global/global.h"
#include "core/types/types.h"
#include "data/shader_template.h"


// @NOTE: mute or show messages from assetmanager
// #define ASSETM_P(txt)  P(txt)
// #define ASSETM_PF(...) PF(__VA_ARGS__)
#define ASSETM_P(txt)
#define ASSETM_PF(...)

void assetm_init();
void assetm_cleanup();

texture_t* assetm_get_texture_by_idx(int idx);
int assetm_get_texture_idx_dbg(const char* name, bool srgb, const char* file, const int line);
texture_t* assetm_get_texture_dbg(const char* name, bool srgb, const char* file, const int line);
void assetm_create_texture_dbg(const char* name, bool srgb, const char* file, const int line);
void assetm_get_texture_data_dbg(const char* name, int* width, int* height, int* channel_num, u8** pixels, const char* file, const int line);
u32 assetm_create_cubemap_dbg(const char* name, const char* ext, const char* file, const int line);
cubemap_t assetm_load_cubemap_hdr_dbg(const char* path, const char* file, const int line);
#define assetm_get_texture_idx(name, srgb)  assetm_get_texture_idx_dbg(name, srgb, __FILE__, __LINE__)
#define assetm_get_texture(name, srgb)      assetm_get_texture_dbg(name, srgb, __FILE__, __LINE__)
#define assetm_create_texture(name, srgb)   assetm_create_texture_dbg(name, srgb, __FILE__, __LINE__)
#define assetm_get_texture_data(name, width, height, channel_num, pixels) \
                                            assetm_get_texture_data_dbg(name, width, height, channel_num, pixels, __FILE__, __LINE__)
#define assetm_create_cubemap(name, ext)    assetm_create_cubemap_dbg(name, ext, __FILE__, __LINE__)
#define assetm_load_cubemap_hdr(path)       assetm_load_cubemap_hdr_dbg(path, __FILE__, __LINE__)

mesh_t* assetm_get_mesh_by_idx(int idx);
int assetm_get_mesh_idx_dbg(const char* name, const char* file, const int line);
mesh_t* assetm_get_mesh_dbg(const char* name, const char* file, const int line);
void assetm_create_mesh_dbg(const char* name, const char* file, const int line);
#define assetm_get_mesh_idx(name) assetm_get_mesh_idx_dbg(name, __FILE__, __LINE__)
#define assetm_get_mesh(name)     assetm_get_mesh_dbg(name, __FILE__, __LINE__)
#define assetm_create_mesh(name)  assetm_create_mesh(name, __FILE__, __LINE__)

shader_t* assetm_get_shader_by_idx_dbg(int idx, const char* file, const int line);
int assetm_get_shader_idx_dbg(shader_template_type type, const char* file, const int line);
shader_t* assetm_get_shader_dbg(shader_template_type type, const char* file, const int line);
void assetm_create_shader_dbg(shader_template_type type, const char* file, const int line);
shader_t assetm_create_shader_from_template_dbg(shader_template_type type, const char* file, const int line);
#define assetm_get_shader_by_idx(idx)               assetm_get_shader_by_idx_dbg(idx, __FILE__, __LINE__)
#define assetm_get_shader_idx(type)                 assetm_get_shader_idx_dbg(type, __FILE__, __LINE__)
#define assetm_get_shader(type)                     assetm_get_shader_dbg(type, __FILE__, __LINE__)
#define assetm_create_shader(type)                  assetm_create_shader_dbg(type, __FILE__, __LINE__)
#define assetm_create_shader_from_template(type)    assetm_create_shader_from_template_dbg(type, __FILE__, __LINE__)


// @TODO: use '_dbg' func's and macros for __FILE__ & __LINE__ as well 
material_t* assetm_get_material_by_idx(int idx);
int assetm_get_material_idx(material_template_type type);
material_t* assetm_get_material(material_template_type type);
void assetm_create_material(material_template_type type);


#endif
