#include "core/types/shader.h"
#include "core/io/file_io.h"
#include "core/debug/debug_opengl.h"

#include "GLAD/glad.h"

#include <stdio.h>
#include <ctype.h>


// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 shader_create(const char* vert_shader_src, const char* frag_shader_src, const char* name, bool* has_error)
{
	// build and compile our shader program
	// ------------------------------------
	
	*has_error = false;

	// vertex shader
	u32 vertexShader = _glCreateShader(GL_VERTEX_SHADER);
	_glShaderSource(vertexShader, 1, &vert_shader_src, NULL);
	_glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	_glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "%s-!!!-> ERROR_VERTEX_COMPILATION: [%s]\n -> %s\n", vert_shader_src, name, infoLog);
		*has_error = true;
	}

	// fragment shader
	u32 fragmentShader = _glCreateShader(GL_FRAGMENT_SHADER);
	_glShaderSource(fragmentShader, 1, &frag_shader_src, NULL);
	_glCompileShader(fragmentShader);

	// check for shader compile errors
	_glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "%s\n-!!!-> ERROR_FRAGMENT_COMPILATION: [%s]\n -> %s\n", frag_shader_src, name, infoLog);
		*has_error = true;
	}

	// link shaders
	u32 shaderProgram = _glCreateProgram();
	_glAttachShader(shaderProgram, vertexShader);
	_glAttachShader(shaderProgram, fragmentShader);
	_glLinkProgram(shaderProgram);

	// check for linking errors
	_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		_glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "-!!!-> ERROR_PROGRAM_LINKING: [%s]\n -> %s\n", name, infoLog);
		*has_error = true;
	}

	// free the shaders
	_glDeleteShader(vertexShader);
	_glDeleteShader(fragmentShader);

	return shaderProgram;
}
// generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 shader_create_tesselation(const char* vert_shader_src, const char* frag_shader_src, const char* tcs_shader_src, const char* tes_shader_src, const char* name, bool* has_error)
{
	// build and compile our shader program
	// ------------------------------------
	
	*has_error = false;

	// vertex shader
	u32 vertex_shader = _glCreateShader(GL_VERTEX_SHADER);
	_glShaderSource(vertex_shader, 1, &vert_shader_src, NULL);
	_glCompileShader(vertex_shader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	_glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		fprintf(stderr, "%s-!!!-> ERROR_VERTEX_COMPILATION: [%s]\n -> %s\n", vert_shader_src, name, infoLog);
		*has_error = true;
	}
	
  // tesselation control shader
	u32 tcs_shader = _glCreateShader(GL_TESS_CONTROL_SHADER);
	_glShaderSource(tcs_shader, 1, &tcs_shader_src, NULL);
	_glCompileShader(tcs_shader);

	// check for shader compile errors
	_glGetShaderiv(tcs_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(tcs_shader, 512, NULL, infoLog);
		fprintf(stderr, "%s-!!!-> ERROR_TESS_CONTROL_COMPILATION: [%s]\n -> %s\n", tcs_shader_src, name, infoLog);
		*has_error = true;
	}
	
  // tesselation control shader
	u32 tes_shader = _glCreateShader(GL_TESS_EVALUATION_SHADER);
	_glShaderSource(tes_shader, 1, &tes_shader_src, NULL);
	_glCompileShader(tes_shader);

	// check for shader compile errors
	_glGetShaderiv(tes_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(tes_shader, 512, NULL, infoLog);
		fprintf(stderr, "%s-!!!-> ERROR_TESS_EVALUATION_COMPILATION: [%s]\n -> %s\n", tes_shader_src, name, infoLog);
		*has_error = true;
	}
  // fragment shader
	u32 fragment_shader = _glCreateShader(GL_FRAGMENT_SHADER);
	_glShaderSource(fragment_shader, 1, &frag_shader_src, NULL);
	_glCompileShader(fragment_shader);

	// check for shader compile errors
	_glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		_glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		fprintf(stderr, "%s\n-!!!-> ERROR_FRAGMENT_COMPILATION: [%s]\n -> %s\n", frag_shader_src, name, infoLog);
		*has_error = true;
	}

	// link shaders
	u32 shader_program = _glCreateProgram();
	_glAttachShader(shader_program, vertex_shader);
	_glAttachShader(shader_program, tcs_shader);
	_glAttachShader(shader_program, tes_shader);
	_glAttachShader(shader_program, fragment_shader);
	_glLinkProgram(shader_program);

	// check for linking errors
	_glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		_glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
		fprintf(stderr, "-!!!-> ERROR_PROGRAM_LINKING: [%s]\n -> %s\n", name, infoLog);
		*has_error = true;
	}

	// free the shaders
	_glDeleteShader(vertex_shader);
	_glDeleteShader(tcs_shader);
	_glDeleteShader(tes_shader);
	_glDeleteShader(fragment_shader);

	return shader_program;
}

// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader_t shader_create_from_file(const char* vert_path, const char* frag_path, uniforms_callback* set_uniforms, const char* name) // const char* vert_path, const char* frag_path
{
	// @TODO: read the shader from file
	// ------------------------------------
	// const char* vert_shader_src_read = read_text_file(vert_path);
	// const char* frag_shader_src_read = read_text_file(frag_path);

	// ---- vert ---- 

	FILE* f;
	char* vert_src;
	u64 len;

	f = fopen(vert_path, "rb");
	if (f == NULL) 
  { ERR("loading vert shader '%s' text-file at: %s\n", name, vert_path); }

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f); 
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(vert_src, len +1);

	// fill text buffer
	fread(vert_src, sizeof(char), len, f);
	assert(strlen(vert_src) > 0);
	fclose(f);
  vert_src[len] = '\0';

	// --------------

	// ---- frag ----

	char* frag_src;
	len = 0;

	f = fopen(frag_path, "rb");
	if (f == NULL) {
		ERR("loading frag shader text-file at: %s\n", frag_path);
		assert(false);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(frag_src, len +1);
	assert(frag_src != NULL);

	// fill text buffer
	fread(frag_src, sizeof(char), len, f);
	assert(strlen(frag_src) > 0);
	fclose(f);
  frag_src[len] = '\0';

	// --------------

	bool has_error = false;
	u32 handle = shader_create(vert_src, frag_src, name, &has_error);

	shader_t s;
	s.handle       = handle;
  s.set_uniforms_f = set_uniforms;
	// s.name = (char*)name;
	s.has_error = has_error;
	SHADER_PF("made shader: name: '%s', handle: %d, has_error: %s\n", name, handle, STR_BOOL(has_error));

	// remember to free the memory allocated by read_text_file()
	free(vert_src);
	free(frag_src);

	return s;
}

// generate a shader-program from a vertex-, fragment-, tesselation control- and tesselation shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader_t shader_create_from_file_tesselation(const char* vert_path, const char* frag_path, const char* tcs_path, const char* tes_path, uniforms_callback* set_uniforms, const char* name)
{
	// @TODO: read the shader from file
	// ------------------------------------
	// const char* vert_shader_src_read = read_text_file(vert_path);
	// const char* frag_shader_src_read = read_text_file(frag_path);

	// ---- vert ---- 

	FILE* f;
	char* vert_src;
	u64 len;

	f = fopen(vert_path, "rb");
	if (f == NULL) 
  { ERR("loading vert shader '%s' text-file at: %s\n", name, vert_path); }

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f); 
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(vert_src, len +1);
	assert(vert_src != NULL);

	// fill text buffer
	fread(vert_src, sizeof(char), len, f);
	assert(strlen(vert_src) > 0);
	fclose(f);
  vert_src[len] = '\0';

	// --------------

  // ---- tcs ----

	char* tcs_src;
	len = 0;

	f = fopen(tcs_path, "rb");
	if (f == NULL) {
		ERR("loading tcs shader text-file at: %s\n", tcs_path);
		assert(false);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(tcs_src, len +1);
	assert(tcs_src != NULL);

	// fill text buffer
	fread(tcs_src, sizeof(char), len, f);
	assert(strlen(tcs_src) > 0);
	fclose(f);
  tcs_src[len] = '\0';
	
	// ---- tes ----

	char* tes_src;
	len = 0;

	f = fopen(tes_path, "rb");
	if (f == NULL) {
		ERR("loading tes shader text-file at: %s\n", tes_path);
		assert(false);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(tes_src, len +1);
	assert(tes_src != NULL);

	// fill text buffer
	fread(tes_src, sizeof(char), len, f);
	assert(strlen(tes_src) > 0);
	fclose(f);
  tes_src[len] = '\0';

  // ---- frag ----

	char* frag_src;
	len = 0;

	f = fopen(frag_path, "rb");
	if (f == NULL) {
		ERR("loading frag shader text-file at: %s\n", frag_path);
		assert(false);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	MALLOC(frag_src, len +1);
	assert(frag_src != NULL);

	// fill text buffer
	fread(frag_src, sizeof(char), len, f);
	assert(strlen(frag_src) > 0);
	fclose(f);
  frag_src[len] = '\0';

	// --------------

	bool has_error = false;
	u32 handle = shader_create_tesselation(vert_src, frag_src, tcs_src, tes_src, name, &has_error);

	shader_t s;
	s.handle       = handle;
  s.set_uniforms_f = set_uniforms;
	// s.name = (char*)name;
	s.has_error = has_error;
	PF("made shader: name: '%s', handle: %d, has_error: %s\n", name, handle, STR_BOOL(has_error));

	// remember to free the memory allocated by read_text_file()
	free(vert_src);
	free(tcs_src);
	free(tes_src);
	free(frag_src);

	return s;
}
typedef enum shader_file_token_type
{
  SHTK_STRING,
  SHTK_VERT,
  SHTK_FRAG,
}shader_tok_type;
#define SHADER_TOK_VAL_MAX 128
typedef struct shader_file_token
{
  shader_tok_type type;
  char val[SHADER_TOK_VAL_MAX];
}shader_tok;

shader_t shader_load_from_path(const char* file_path, const char* name)
{
  if (!file_io_check_exists(file_path)) { ERR("shader file doesnt exist. '%s': '%s'", name, file_path); }

  int txt_len = 0;
  char* txt = file_io_read_len(file_path, &txt_len);

  shader_tok t[64];
  int t_len = 0;
  shader_tok tok;

  for (int i = 0; i < txt_len; ++i)
  {
    if (isspace(txt[i])) { continue; }

    if (txt[i +0] == '[' &&
        txt[i +1] == 'v' &&
        txt[i +2] == 'e'&&
        txt[i +3] == 'r'&&
        txt[i +4] == 't'&&
        txt[i +5] == ']'&&
        isspace(txt[i +6]))
    {
      tok.type = SHTK_VERT;
      t[t_len++] = tok;
      i += 5;
      P("[VERT]");
      continue;
    }
    if (txt[i + 0] == '[' &&
        txt[i + 1] == 'f' &&
        txt[i + 2] == 'r' &&
        txt[i + 3] == 'a' &&
        txt[i + 4] == 'g' &&
        txt[i + 5] == ']' &&
        isspace(txt[i + 6]))
    {
      tok.type = SHTK_FRAG;
      t[t_len++] = tok;
      i += 5;
      P("[FRAG]");
      continue;
    }

    // read strings
    int j = 0;
    while (i +j < txt_len && 
        (isalnum(txt[i + j]) || txt[i + j] == '.' || txt[i + j] == '_'))
    { j++; }
    if (j > 1)
    {
      char end = txt[i +j];
      txt[i +j] = '\0';
      strcpy(tok.val, txt + i);
      P_STR(tok.val);
      txt[i +j] = end;
      tok.type = SHTK_STRING;
      t[t_len++] = tok;
      i += j;
      continue;
    }
  }

  int vert = -1;
  int frag = -1;
  for (int i = 0; i < t_len -1; ++i)
  {
    if (t[i].type == SHTK_VERT && t[i + 1].type == SHTK_STRING)
    { vert = i + 1; }
    if (t[i].type == SHTK_FRAG && t[i + 1].type == SHTK_STRING)
    { frag = i + 1; }
  }
  if (vert <= -1 || frag <= -1) { ERR("vert or frag shader missing"); }
 
  // -- tmp --
  const int NAME_SIZE = 16;
  char vert_name[NAME_SIZE], frag_name[NAME_SIZE];
  strcpy(vert_name, t[vert].val);
  strcpy(frag_name, t[frag].val);
  SPRINTF(NAME_SIZE, t[vert].val, "assets/shaders/%s", vert_name);
  SPRINTF(NAME_SIZE, t[frag].val, "assets/shaders/%s", frag_name);

  shader_t s = shader_create_from_file(t[vert].val, t[frag].val, NULL, name);

  return s;
}

// activate / use the shader 
void shader_use(shader_t* s)
{
  _glUseProgram(s->handle);
}
void shader_delete(shader_t* s)
{
	_glDeleteProgram(s->handle);
}

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(shader_t* s, const char* name, int value)
{
	_glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set an integer in the shader
void shader_set_int(shader_t* s, const char* name, int value)
{
	_glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set a float in the shader
void shader_set_float(shader_t* s, const char* name, f32 value)
{
	_glUniform1f(glGetUniformLocation(s->handle, name), value);
}
// set a vec2 in the shader
void shader_set_vec2_f(shader_t* s, const char* name, f32 x, f32 y)
{
	_glUniform2f(glGetUniformLocation(s->handle, name), x, y);
}
// set a vec2 in the shader
void shader_set_vec2(shader_t* s, const char* name, vec2 v)
{
	_glUniform2f(glGetUniformLocation(s->handle, name), v[0], v[1]);
}
// set a vec3 in the shader
void shader_set_vec3_f(shader_t* s, const char* name, f32 x, f32 y, f32 z)
{
	_glUniform3f(glGetUniformLocation(s->handle, name), x, y, z);
}
// set a vec3 in the shader
void shader_set_vec3(shader_t* s, const char* name, vec3 v)
{
	_glUniform3f(glGetUniformLocation(s->handle, name), v[0], v[1], v[2]);
}
// set a matrix 4x4 in the shader
void shader_set_mat4(shader_t* s, const char* name, mat4 value)
{
	u32 transformLoc = _glGetUniformLocation(s->handle, name);
	_glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value[0]);
}
