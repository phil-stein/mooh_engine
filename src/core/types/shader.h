#ifndef SHADER_H
#define SHADER_H

#include "global/global.h"
#include "math/math_inc.h"


struct shader_t;
typedef void (uniforms_callback)(struct shader_t* shader, int tex_idx);

typedef struct shader_t
{
	bool has_error;
	u32 handle;
  uniforms_callback* set_uniforms;
}shader_t;


// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 shader_create(const char* vert_shader_src, const char* frag_shader_src, const char* name, bool* has_error);
// generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 shader_create_tesselation(const char* vert_shader_src, const char* frag_shader_src, const char* tcs_shader_source, const char* tes_shader_source, const char* name, bool* has_error);
// generate a shader-program from a vertex- and fragment-shader
// give the filepath to the vert / frag shader as the arguments
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader_t shader_create_from_file(const char* vert_path, const char* frag_path, uniforms_callback* set_uniforms, const char* name);

// generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader_t shader_create_from_file_tesselation(const char* vert_path, const char* frag_path, const char* tcs_path, const char* tes_path, uniforms_callback* set_uniforms, const char* name);

  // shader_t shader_load_from_path(const char* file_path, const char* name);

// activate / use the shader 
void shader_use(shader_t* s);

// "free's" the shader program
// !!! only deletes the opngl handle not the name, etc.
void shader_delete(shader_t* s);

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(shader_t* s, const char* name, int value);
// set an integer in the shader
void shader_set_int(shader_t* s, const char* name, int value);
// set a float in the shader
void shader_set_float(shader_t* s, const char* name, f32 value);
// set a vec2 in the shader
void shader_set_vec2_f(shader_t* s, const char* name, f32 x, f32 y);
// set a vec2 in the shader
void shader_set_vec2(shader_t* s, const char* name, vec2 v);
// set a vec3 in the shader
void shader_set_vec3_f(shader_t* s, const char* name, f32 x, f32 y, f32 z);
// set a vec3 in the shader
void shader_set_vec3(shader_t* s, const char* name, vec3 v);
// set a matrix 4x4 in the shader
void shader_set_mat4(shader_t* s, const char* name, mat4 value);

#endif
