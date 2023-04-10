#ifndef CORE_SHADER_H
#define CORE_SHADER_H

#include "global/global.h"
#include "math/math_inc.h"


// @NOTE: mute or show shader messages
// #define SHADER_PF(...) PF(__VA_ARGS__)
#define SHADER_PF(...)


// @DOC: callbacl for func called before shader used to render, for setting uniforms
struct shader_t;
typedef void (uniforms_callback)(struct shader_t* shader, int tex_idx);

typedef struct shader_t
{
	bool has_error;                    // whether the shader had a error during compilation
	u32  handle;                       // handle used for opengl calls
  uniforms_callback* set_uniforms_f; // gets called before shader call to set uniforms, otherwise NULL
}shader_t;


// @DOC: generate a shader-program from a vertex- and fragment-shader
//       returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
//       vert_shader_src: source text of shader file
//       frag_shader_src: source text of shader file
//       name:            name for the shader
//       has_error:       true / false after compilation
u32 shader_create(const char* vert_shader_src, const char* frag_shader_src, const char* name, bool* has_error);
// @DOC: generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
//       returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
//       vert_shader_src: source text of shader file
//       frag_shader_src: source text of shader file
//       tcs_shader_src:  source text of shader file
//       tes_shader_src:  source text of shader file
//       name:            name for the shader
//       has_error:       true / false after compilation
u32 shader_create_tesselation(const char* vert_shader_src, const char* frag_shader_src, const char* tcs_shader_source, const char* tes_shader_source, const char* name, bool* has_error);
// @DOC: generate a shader-program from a vertex- and fragment-shader
//       give the filepath to the vert / frag shader as the arguments
//       returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
//       vert_path:    path to shader file
//       frag_path:    path to shader file
//       set_uniforms: func-ptr for shader_t.set_uniforms_f
//       name:         name for the shader
shader_t shader_create_from_file(const char* vert_path, const char* frag_path, uniforms_callback* set_uniforms, const char* name);

// @DOC: generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
//       returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
//       vert_path:    path to shader file
//       frag_path:    path to shader file
//       tcs_path:     path to shader file
//       tes_path:     path to shader file
//       set_uniforms: func-ptr for shader_t.set_uniforms_f
//       name:         name for the shader
shader_t shader_create_from_file_tesselation(const char* vert_path, const char* frag_path, const char* tcs_path, const char* tes_path, uniforms_callback* set_uniforms, const char* name);

  // shader_t shader_load_from_path(const char* file_path, const char* name);

// @DOC: activate / use the shader 
//       s: shader to be activated
void shader_use(shader_t* s);

// @DOC: "free's" the shader program
//       !!! only deletes the opngl handle not the name, etc.
//       s: shader to be deleted
void shader_delete(shader_t* s);

// @DOC: set a bool in the shader 
//       the given int is used as the bool ( 0/1 )
//       s:     shader 
//       name:  name of uniform variable
//       value: value to be set
void shader_set_bool(shader_t* s, const char* name, int value);
// @DOC: set an integer in the shader
//       s:     shader 
//       name:  name of uniform variable
//       value: value to be set
void shader_set_int(shader_t* s, const char* name, int value);
// @DOC: set a float in the shader
//       s:     shader 
//       name:  name of uniform variable
//       value: value to be set
void shader_set_float(shader_t* s, const char* name, f32 value);
// @DOC: set a vec2 in the shader, via floats
//       s:     shader 
//       name:  name of uniform variable
//       x:     x of vec
//       y:     y of vec
void shader_set_vec2_f(shader_t* s, const char* name, f32 x, f32 y);
// set a vec2 in the shader, via vec2 float array
//       s:     shader 
//       name:  name of uniform variable
//       v:     vec2
void shader_set_vec2(shader_t* s, const char* name, vec2 v);
// @DOC: set a vec3 in the shader, via floats
//       s:     shader 
//       name:  name of uniform variable
//       x:     x of vec
//       y:     y of vec
//       z:     z of vec
void shader_set_vec3_f(shader_t* s, const char* name, f32 x, f32 y, f32 z);
// @DOC: set a vec3 in the shader, via vec3 float array
//       s:     shader 
//       name:  name of uniform variable
//       v:     vec3
void shader_set_vec3(shader_t* s, const char* name, vec3 v);
// @DOC: set a matrix 4x4 in the shader, via mat4 float[][] array
//       s:     shader 
//       name:  name of uniform variable
//       value: mat4, float[][]
void shader_set_mat4(shader_t* s, const char* name, mat4 value);

#endif
