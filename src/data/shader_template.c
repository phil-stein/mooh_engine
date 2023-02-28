#include "data/shader_template.h"
#include "core/types/shader.h"


// -- set uniform callbacks --

void test_shader_set_uniforms(shader_t* shader, int tex_idx)
{
  shader_set_vec3(shader, "fade_color", VEC3_XYZ(1, 0, 1));
}


const shader_template_t shader_table[] = 
{
  // basic
  {
    .name  = "basic_shader",
    .vert  = "basic.vert",
    .frag  = "basic.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // shadow map
  {
    .name  = "shadow_map_shader",
    .vert  = "shadow_map.vert",
    .frag  = "empty.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // deferred
  {
    .name  = "deferred_shader",
    .vert  = "basic.vert",
    .frag  = "deferred.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // skybox
  {
    .name  = "skybox_shader",
    .vert  = "cubemap/cube_map.vert",
    .frag  = "cubemap/cube_map.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // shadow pass
  {
    .name  = "shadow_pass_shader",
    .vert  = "screen.vert",
    .frag  = "shadow_pass.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // lighting
  {
    .name  = "lighting_shader",
    .vert  = "screen.vert",
    .frag  = "pbr.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // post-fx
  {
    .name  = "post_fx_shader",
    .vert  = "screen.vert",
    .frag  = "post_fx.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // brdf lut
  {
    .name  = "brdf_lut_shader",
    .vert  = "cubemap/brdf_lut.vert",
    .frag  = "cubemap/brdf_lut.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // mouse pick
  {
    .name  = "mouse_pick_shader",
    .vert  = "basic.vert",
    .frag  = "mouse_picking.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },
  // terrain
  {
    .name  = "terrain_shader",
    .vert  = "terrain.vert",
    .frag  = "terrain.frag",
    .unlit = false,
    .set_uniforms_f = NULL,
  },

  // -- custom --
  {
    .name  = "test_shader",
    .vert  = "basic.vert",
    .frag  = "custom/test.frag", 
    .unlit = false,
    .set_uniforms_f = test_shader_set_uniforms,
  },
};
const int shader_table_len = sizeof(shader_table) / sizeof(shader_table[0]);

const shader_template_t* shader_template_get(int idx)
{
  ERR_CHECK(idx < shader_table_len, "shader template idx too high: %d", idx);
  ERR_CHECK(idx >= 0,                "shader template idx too low: %d", idx);
  return &shader_table[idx]; 
}

const shader_template_t* shader_template_get_all(int* len)
{
  *len = shader_table_len;
  return shader_table;
}



