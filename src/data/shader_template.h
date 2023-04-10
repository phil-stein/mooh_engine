#ifndef DATA_SHADER_TEMPLATE_H 
#define DATA_SHADER_TEMPLATE_H 

#include "global/global.h"
#include "core/types/shader.h"

// @DOC: act as index for shader_template_get()
//       ! if out of order shaders will be loaded as the wrong shader
typedef enum shader_template_type
{
  SHADER_TEMPLATE_NONE = -1,  // used for custom shader on material system
  
  SHADER_TEMPLATE_BASIC,
  SHADER_TEMPLATE_SHADOW_MAP,
  SHADER_TEMPLATE_DEFERRED,
  SHADER_TEMPLATE_SKYBOX,
  SHADER_TEMPLATE_SHADOW_PASS,
  SHADER_TEMPLATE_LIGHTING,
  SHADER_TEMPLATE_POST_FX,
  SHADER_TEMPLATE_BRDF_LUT,
  SHADER_TEMPLATE_MOUSE_PICK,
  SHADER_TEMPLATE_TERRAIN,
  
  // -- custom --

  SHADER_TEMPLATE_TEST,

}shader_template_type;

// @DOC: template for shader
//       specifies all data needed to make shader
typedef struct shader_template_t
{
  const char* name; 
  const char* vert; 
  const char* frag;
  bool unlit;
  uniforms_callback* set_uniforms_f;
}shader_template_t;

// @DOC: get pointer to shader template by its index, see shader_template_type
//       idx: index for template, use shader_template_type
const shader_template_t* shader_template_get(int idx);

// @DOC: get all templates in arr
//       len: gets set to arr's length
const shader_template_t* shader_template_get_all(int* len);

#endif
