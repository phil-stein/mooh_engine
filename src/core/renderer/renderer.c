#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"
#include "core/renderer/renderer_extra.h"
#include "core/window.h"
#include "core/camera.h"
#include "core/state.h"
#include "core/io/assetm.h"
#include "core/input.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/debug/debug_opengl.h"
#include "data/shader_template.h"


#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"


// @TODO: put all these vars in a struct and make a _get() func

// ---- vars ----

// vao, vbo for the quad used in render_quad()
u32 skybox_vao, skybox_vbo;

const f32 exposure    = 1.25f;

static core_data_t* core_data;


void renderer_init()
{
  core_data = core_data_get();

  // cube map -------------------------------------------------------------------------------------

  f32 skybox_verts[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  };

  //  cube vao
  _glGenVertexArrays(1, &skybox_vao);
  _glGenBuffers(1, &skybox_vbo);
  _glBindVertexArray(skybox_vao);
  _glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  _glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(f32), &skybox_verts, GL_STATIC_DRAW); // skybox_verts is 108 long
  _glEnableVertexAttribArray(0);
  _glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
  // ----------------------------------------------------------------------------------------------

  // int texture_units;
  // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units); 
  // P_INT(texture_units);

  // -- opengl state --
  _glEnable(GL_DEPTH_TEST); // enable the z-buffer
  _glEnable(GL_CULL_FACE);
  _glCullFace(GL_BACK);
  _glEnable(GL_BLEND); // enable blending of transparent texture
  //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  _glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  _glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
  // fix seams betweencubemap faces
  _glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}


void renderer_update()
{
  core_data->draw_calls_total       = 0;
  core_data->draw_calls_screen_quad = 0;
  core_data->draw_calls_deferred    = 0;
  core_data->draw_calls_shadow      = 0;
    
	int w, h;
	window_get_size(&w, &h);
  // glViewport(0, 0, w, h);
 
  #ifdef EDITOR
  // -- opengl state --
  _glEnable(GL_DEPTH_TEST); // enable the z-buffer
  _glEnable(GL_CULL_FACE);
  _glCullFace(GL_BACK);
  _glEnable(GL_BLEND); // enable blending of transparent texture
  //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  _glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  _glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
  #endif

  // -- background --
	// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---- draw ----

  // @TODO: get this in advance
  int world_len = 0;
  int world_dead_len = 0;
  entity_t* world = state_entity_get_arr(&world_len, &world_dead_len);
  int dir_lights_len = 0;
  dir_light_t* dir_lights = state_dir_light_get_arr(&dir_lights_len);
  int point_lights_len = 0;
  int point_lights_dead_len = 0;
  point_light_t* point_lights = state_point_light_get_arr(&point_lights_len, &point_lights_dead_len);
  

  TIMER_START("shadow maps");
  // shadow maps ------------------------------------------------------------
  
  mat4 proj; 
  mat4_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 12.5f, proj);  // shadow_near_plane, shadow_far_plane

  mat4 view;
  vec3 center = { 0.0f,  0.0f,  0.0f };
  vec3 up     = { 0.0f,  1.0f,  0.0f };


  // @TODO: shadows follow camera
  vec3 light_pos, c_forward;
  vec3_copy(core_data->cam.pos, light_pos);   // camera_get_pos(light_pos);
  vec3_add(light_pos, VEC3_XYZ(0, 5, 0), light_pos);
  vec3_copy(core_data->cam.front, c_forward); // camera_get_front(c_forward);
  vec3_mul_f(c_forward, 5.0f, c_forward);
  vec3_add(light_pos, c_forward, light_pos);

  int shadow_caster_counter = 0;
  for (int i = 0; i < dir_lights_len; ++i)
  {
    dir_light_t* l = &dir_lights[i];
    if (!l->cast_shadow) { continue; }
    shadow_caster_counter++;
    glViewport(0, 0, l->shadow_map_x, l->shadow_map_y);
    framebuffer_bind(&l->fb_shadow); // &fb_lighting
    glClear(GL_DEPTH_BUFFER_BIT);

    vec3 inv_dir;
    vec3_mul_f(l->dir, -1.0f, inv_dir);
    vec3_add(light_pos, inv_dir, center);
    mat4_lookat(light_pos, center, up, view);

    mat4_copy(view, l->view); // for the lighting calc 
    mat4_copy(proj, l->proj); // for the lighting calc 

    entity_t* e;
    shader_use(&core_data->shadow_shader);
    shader_set_mat4(&core_data->shadow_shader, "view", view);
    shader_set_mat4(&core_data->shadow_shader, "proj", proj);
    for (int i = 0; i < world_len; ++i)
    {
      e = &world[i];
      if (e->is_dead || e->mesh < 0 || e->mat < 0) { continue; }
  
      shader_set_mat4(&core_data->shadow_shader, "model", e->model);

      mesh_t* mesh = assetm_get_mesh_by_idx(e->mesh);
      DRAW_MESH(mesh); 
      core_data->draw_calls_total++;
      core_data->draw_calls_shadow++;
    }
    for (int i = 0; i < core_data->terrain_chunks_len; ++i) 
    { 
      if (!core_data->terrain_chunks[i].loaded || !core_data->terrain_chunks[i].visible) { continue; }
      shader_set_mat4(&core_data->shadow_shader, "model", core_data->terrain_chunks[i].model);
      renderer_draw_terrain_mesh(&core_data->terrain_chunks[i]); 
      core_data->draw_calls_shadow++; // draw terrain inc's draw_calls_total
    }
    framebuffer_unbind();
  }
  core_data->show_shadows = shadow_caster_counter > 0 ? true : false;
  _glViewport(0, 0, w, h);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  TIMER_STOP();

  // view & proj ------------------------------------------------------------
  
  // mat4 view;
  camera_get_view_mat(view);
  mat4 view_no_pos;
  mat4_copy(view, view_no_pos);
  mat4_set_pos(0, 0, 0, view_no_pos);

  camera_get_proj_mat(w, h, proj);


  TIMER_START("deferred");
  // deferred ---------------------------------------------------------------
  
  if (core_data->wireframe_mode_enabled == true)
	{ _glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else
	{ _glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

  framebuffer_bind(&core_data->fb_deferred);
  // framebuffer_bind(&core_data->fb_deferred_msaa);
  {
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    entity_t* e;
    for (int i = 0; i < world_len; ++i)
    {
      e = &world[i];
      if (e->is_dead || e->mesh < 0 || e->mat < 0) { continue; }

      // ---- shader & draw call -----	
      material_t* mat = assetm_get_material_by_idx(e->mat);

      // @TODO: do this outside the for-loop
      shader_t* mat_shader = &core_data->deferred_shader;
      if (mat->shader >= 0) { mat_shader = assetm_get_shader_by_idx(mat->shader); }
      shader_use(mat_shader);
      shader_set_int(mat_shader, "albedo", 0);
      shader_set_int(mat_shader, "norm", 1);
      shader_set_int(mat_shader, "roughness", 2);
      shader_set_int(mat_shader, "metallic", 3);

      shader_set_vec3(mat_shader, "tint", mat->tint);
      int tex_idx = 0;
      _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->albedo)->handle); 
      
      _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->normal)->handle); 
      
      _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->roughness)->handle); 
      
      _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->metallic)->handle);

    
      ERR_CHECK(tex_idx <= 31, "bound GL_TEXTURE%d, max: 31\n", tex_idx);

      shader_set_float(mat_shader, "roughness_f", mat->roughness_f);
      shader_set_float(mat_shader, "metallic_f", mat->metallic_f);

      vec2 tile; 
      vec2_copy(mat->tile, tile);
      if (mat->tile_by_scl) 
      { 
        f32 uv_scl = ( e->scl[0] + e->scl[1] + e->scl[2] ) / 3;
        vec2_mul_f(tile, uv_scl, tile); 
      }
      vec2_mul_f(tile, mat->tile_scl, tile);
      shader_set_vec2(mat_shader, "uv_tile", tile);

      shader_set_mat4(mat_shader, "model", e->model);  // model gets updated in shadow map
      shader_set_mat4(mat_shader, "view", view);
      shader_set_mat4(mat_shader, "proj", proj);

      if (mat_shader->set_uniforms_f != NULL) { mat_shader->set_uniforms_f(mat_shader, tex_idx); }

      mesh_t* mesh = assetm_get_mesh_by_idx(e->mesh);
      DRAW_MESH(mesh);
      core_data->draw_calls_total++;
      core_data->draw_calls_deferred++;
    }
    
    for (int i = 0; i < core_data->terrain_chunks_len; ++i) 
    { 
      if (!core_data->terrain_chunks[i].loaded || !core_data->terrain_chunks[i].visible) { continue; }
      renderer_draw_terrain(view, proj, &core_data->terrain_chunks[i]); 
      core_data->draw_calls_deferred++; // draw terrain inc's draw_calls_total
    }
    
    // skybox -----------------------------------------------------------------
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

    shader_use(&core_data->skybox_shader);
    shader_set_mat4(&core_data->skybox_shader, "view", view_no_pos);
    shader_set_mat4(&core_data->skybox_shader, "proj", proj);

    // skybox cube
    _glBindVertexArray(skybox_vao);
    _glActiveTexture(GL_TEXTURE0);
    shader_set_int(&core_data->skybox_shader, "cube_map", 0);
    _glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.environment);
    _glDrawArrays(GL_TRIANGLES, 0, 36);
    _glBindVertexArray(0);
    _glDepthFunc(GL_LESS); // set depth function back to default
    core_data->draw_calls_total++;
    // ------------------------------------------------------------------------
  }
  // framebuffer_bind(&core_data->fb_deferred);
  // framebuffer_blit_gbuffer_multisampled(&core_data->fb_deferred_msaa, &core_data->fb_deferred);
  framebuffer_unbind();
  TIMER_STOP();

  TIMER_START("shadow pass");
  // shadow pass -----------------------------------------
  framebuffer_bind(&core_data->fb_shadow_pass);
  {    
    if (core_data->wireframe_mode_enabled == true)
    { _glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _glDisable(GL_DEPTH_TEST);
    shader_use(&core_data->shadow_pass_shader);
    
    int  tex_index = 0;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer03); 
    shader_set_int(&core_data->shadow_pass_shader, "normal", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer04); 
    shader_set_int(&core_data->shadow_pass_shader, "position", tex_index);
    tex_index++;

    // lights ----------------------------------------------
    // @UNSURE: limit shadow casters to 1 ??? 
    #define BUF_SIZE1 28
    char buffer[BUF_SIZE1];
    shader_set_int(&core_data->shadow_pass_shader, "shadows_len", core_data->show_shadows ? 1 : 0);
    if (core_data->show_shadows)
    {
      for (int i = 0; i < 1; ++i)
      {
        dir_light_t* light = &dir_lights[i];

        int idx = i; //  - disabled_lights;
        SPRINTF(BUF_SIZE1, buffer, "shadows[%d].direction", idx);
        shader_set_vec3(&core_data->shadow_pass_shader, buffer, light->dir);

        SPRINTF(BUF_SIZE1, buffer, "shadows[%d].shadow_map", idx);
        _glActiveTexture(GL_TEXTURE0 + tex_index);
        _glBindTexture(GL_TEXTURE_2D, light->fb_shadow.buffer);
        shader_set_int(&core_data->shadow_pass_shader, buffer, tex_index);
        tex_index++;
        SPRINTF(BUF_SIZE1, buffer, "shadows[%d].view", idx);
        shader_set_mat4(&core_data->shadow_pass_shader, buffer, light->view);
        SPRINTF(BUF_SIZE1, buffer, "shadows[%d].proj", idx);
        shader_set_mat4(&core_data->shadow_pass_shader, buffer, light->proj);

        ERR_CHECK(tex_index <= 31, "bound GL_TEXTURE%d, max: 31\n", tex_index);
      }
    }
    #undef BUF_SIZE1
    // -----------------------------------------------------

    _glBindVertexArray(core_data->quad_vao);
    _glDrawArrays(GL_TRIANGLES, 0, 6);
    _glEnable(GL_DEPTH_TEST);
    core_data->draw_calls_total++;
    core_data->draw_calls_screen_quad++;
  }
  framebuffer_unbind();
  TIMER_STOP();

  TIMER_START("lighting");
  // lighting --------------------------------------------
  framebuffer_bind(&core_data->fb_lighting);
  {    
    if (core_data->wireframe_mode_enabled == true)
    { _glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _glDisable(GL_DEPTH_TEST);
    shader_use(&core_data->lighting_shader);
    
    // vec3 cam_pos; vec3_copy(core_data->cam.pos, cam_pos); // camera_get_pos(cam_pos);
    shader_set_vec3(&core_data->lighting_shader, "view_pos", core_data->cam.pos); // cam_pos
    shader_set_float(&core_data->lighting_shader, "cube_map_intensity", core_data->cube_map.intensity);
    
    int  tex_index = 0;

    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.irradiance);
    shader_set_int(&core_data->lighting_shader, "irradiance_map", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.prefilter);
    shader_set_int(&core_data->lighting_shader, "prefilter_map", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->brdf_lut);
    shader_set_int(&core_data->lighting_shader, "brdf_lut", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer);
    shader_set_int(&core_data->lighting_shader, "color", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer02);
    shader_set_int(&core_data->lighting_shader, "material", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer03); 
    shader_set_int(&core_data->lighting_shader, "normal", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index);
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer04); 
    shader_set_int(&core_data->lighting_shader, "position", tex_index);
    tex_index++;
    _glActiveTexture(GL_TEXTURE0 + tex_index); 
    _glBindTexture(GL_TEXTURE_2D, core_data->fb_shadow_pass.buffer); 
    shader_set_int(&core_data->lighting_shader, "shadow", tex_index);
    tex_index++;

    ERR_CHECK(tex_index <= 31, "bound GL_TEXTURE%d, max: 31\n", tex_index);

    // lights ----------------------------------------------
    #define BUF_SIZE2 28
    char buffer[BUF_SIZE2];
    // int  shadow_idx = 0;
    shader_set_int(&core_data->lighting_shader, "dir_lights_len", dir_lights_len);
    for (int i = 0; i < dir_lights_len; ++i)
    {
      dir_light_t* light = &dir_lights[i];

      int idx = i; //  - disabled_lights;
      SPRINTF(BUF_SIZE2, buffer, "dir_lights[%d].direction", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, light->dir);

      vec3 color;
      vec3_mul_f(light->color, light->intensity, color);
      SPRINTF(BUF_SIZE2, buffer, "dir_lights[%d].color", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, color);
    }
    shader_set_int(&core_data->lighting_shader, "point_lights_len", point_lights_len - point_lights_dead_len);
    int point_lights_disabled = 0;
    for (int i = 0; i < point_lights_len; ++i)
    {
      point_light_t* light = &point_lights[i];
      if (light->is_dead) { point_lights_disabled++; continue; }
      entity_t* e = state_entity_get(light->entity_id);
      vec3 l_pos; vec3_add(e->pos, light->offset, l_pos);

      int idx = i - point_lights_disabled;
      SPRINTF(BUF_SIZE2, buffer, "point_lights[%d].pos", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, l_pos); // light->pos_ptr);
      SPRINTF(BUF_SIZE2, buffer, "point_lights[%d].color", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, light->color);
      SPRINTF(BUF_SIZE2, buffer, "point_lights[%d].intensity", idx);
      shader_set_float(&core_data->lighting_shader, buffer, light->intensity);
    }
    #undef BUF_SIZE2
    // -----------------------------------------------------

    _glBindVertexArray(core_data->quad_vao);
    _glDrawArrays(GL_TRIANGLES, 0, 6);
    _glEnable(GL_DEPTH_TEST);
    core_data->draw_calls_total++;
    core_data->draw_calls_screen_quad++;
  }
  framebuffer_unbind();
  TIMER_STOP();

#ifdef OUTLINE
  TIMER_FUNC(renderer_extra_draw_scene_outline());
#endif // EDITOR

  TIMER_START("post fx");
  // post fx ---------------------------------------------

  _glClear(GL_COLOR_BUFFER_BIT);

  _glDisable(GL_DEPTH_TEST);
  shader_use(&core_data->post_fx_shader);
  shader_set_float(&core_data->post_fx_shader, "exposure", exposure);
  _glActiveTexture(GL_TEXTURE0);
  _glBindTexture(GL_TEXTURE_2D, core_data->fb_lighting.buffer);  // fb_mouse_pick.buffer); // dir_lights[0].fb_shadow.buffer // core_data->fb_deferred.buffer03);
  shader_set_int(&core_data->post_fx_shader, "tex", 0);

#ifdef OUTLINE
  _glActiveTexture(GL_TEXTURE1);
  _glBindTexture(GL_TEXTURE_2D, core_data->fb_outline.buffer);  // fb_mouse_pick.buffer); // dir_lights[0].fb_shadow.buffer // core_data->fb_deferred.buffer03);
  shader_set_int(&core_data->post_fx_shader, "outline", 1);
#endif // OUTLINE

  _glBindVertexArray(core_data->quad_vao);
  _glDrawArrays(GL_TRIANGLES, 0, 6);
  _glEnable(GL_DEPTH_TEST);
  core_data->draw_calls_total++;
  core_data->draw_calls_screen_quad++;

  TIMER_STOP();

}

void renderer_draw_terrain(mat4 view, mat4 proj, terrain_chunk_t* chunk)
{
  if (!chunk->loaded || !chunk->visible) { return; }
	
  // ---- mvp ----
  if (chunk->is_moved)
  { 
    mat4_make_model(chunk->pos, VEC3(0), VEC3(core_data->terrain_scl), chunk->model); 
    chunk->is_moved = false;
  }
  // mat4_make_model(core_data->terrain_pos, core_data->terrain_rot, core_data->terrain_scl, core_data->terrain_model); 


	// ---- shader & draw call -----
  #define BUF_SIZE0 64
  char buf[BUF_SIZE0];
  u32 tex_idx = 0;
  for (u32 i = 0; i < core_data->terrain_materials_len; ++i)
  {
    material_t* mat = assetm_get_material_by_idx(core_data->terrain_materials[i]);
    
    shader_use(&core_data->terrain_shader);
    
    SPRINTF(BUF_SIZE0, buf, "materials[%d].albedo", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->albedo)->handle); 
    
    SPRINTF(BUF_SIZE0, buf, "materials[%d].normal", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx); 
    _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->normal)->handle); 
    
    SPRINTF(BUF_SIZE0, buf, "materials[%d].roughness", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->roughness)->handle); 

    SPRINTF(BUF_SIZE0, buf, "materials[%d].metallic", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    _glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    _glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->metallic)->handle);

    SPRINTF(BUF_SIZE0, buf, "materials[%d].tint", i);
    shader_set_vec3(&core_data->terrain_shader, buf, mat->tint);

    SPRINTF(BUF_SIZE0, buf, "materials[%d].roughness_f", i);
    shader_set_float(&core_data->terrain_shader, buf, mat->roughness_f);
    SPRINTF(BUF_SIZE0, buf, "materials[%d].metallic_f", i);
    shader_set_float(&core_data->terrain_shader, buf, mat->metallic_f);
  }
  shader_set_mat4(&core_data->terrain_shader, "model", chunk->model);  // model gets updated in shadow map
  shader_set_mat4(&core_data->terrain_shader, "view", view);
  shader_set_mat4(&core_data->terrain_shader, "proj", proj);
  #undef BUF_SIZE0

  // draw mesh
  // glCullFace(GL_FRONT);
  _glBindVertexArray(chunk->vao);
  // render the mesh triangle strip by triangle strip - each row at a time
  for(u32 strip = 0; strip < chunk->strips_num; ++strip)
  {
    _glDrawElements(GL_TRIANGLE_STRIP,       // primitive type
        chunk->verts_per_strip,             // number of indices to render
        GL_UNSIGNED_INT,                    // index data type
        (void*)(sizeof(u32)
          * chunk->verts_per_strip
          * strip)); // offset to starting index
    core_data->draw_calls_total++;
  }
  // glCullFace(GL_BACK);
}

void renderer_draw_terrain_mesh(terrain_chunk_t* chunk)
{
  // draw mesh
  _glBindVertexArray(chunk->vao);
  // render the mesh triangle strip by triangle strip - each row at a time
  for(u32 strip = 0; strip < chunk->strips_num; ++strip)
  {
    _glDrawElements(GL_TRIANGLE_STRIP,       // primitive type
        chunk->verts_per_strip,             // number of indices to render
        GL_UNSIGNED_INT,                    // index data type
        (void*)(sizeof(u32)
          * chunk->verts_per_strip
          * strip)); // offset to starting index
    core_data->draw_calls_total++;
  }
}

