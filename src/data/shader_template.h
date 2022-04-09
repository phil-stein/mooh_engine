#ifndef SHADER_TEMPLATE_H 
#define SHADER_TEMPLATE_H 

#include "global/global.h"
#include "core/types/shader.h"

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

typedef struct shader_template_t
{
  const char* name; 
  const char* vert; 
  const char* frag;
  bool unlit;
  uniforms_callback* set_uniforms;
}shader_template_t;

const shader_template_t* shader_template_get(int idx);
const shader_template_t* shader_template_get_all(int* len);

#endif
