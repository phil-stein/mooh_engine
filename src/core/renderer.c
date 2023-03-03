#include "core/renderer.h"
#include "core/renderer_direct.h"
#include "core/window.h"
#include "core/camera.h"
#include "core/state.h"
#include "core/assetm.h"
#include "core/input.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "data/shader_template.h"

#include "editor/app.h"   // big bad only tmp

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"


// @TODO: put all these vars in a struct and make a _get() func

// ---- vars ----

// vao, vbo for the quad used in render_quad()
u32 skybox_vao, skybox_vbo;

u32 brdf_lut;

const f32 exposure    = 1.25f;

static core_data_t* core_data;

#define DRAW_MESH(_mesh)                                                                \
      {                                                                                 \
        glBindVertexArray(_mesh->vao);                                                  \
        if (_mesh->indexed)                                                             \
        { glDrawElements(GL_TRIANGLES, _mesh->indices_count, GL_UNSIGNED_INT, 0); }     \
        else                                                                            \
        { glDrawArrays(GL_TRIANGLES, 0, _mesh->verts_count); }                          \
      }

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
  glGenVertexArrays(1, &skybox_vao);
  glGenBuffers(1, &skybox_vbo);
  glBindVertexArray(skybox_vao);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(f32), &skybox_verts, GL_STATIC_DRAW); // skybox_verts is 108 long
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
  // ----------------------------------------------------------------------------------------------

  brdf_lut = renderer_gen_brdf_lut(); // @TODO: put in core_data

  // int texture_units;
  // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units); 
  // P_INT(texture_units);

  // -- opengl state --
  glEnable(GL_DEPTH_TEST); // enable the z-buffer
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND); // enable blending of transparent texture
  //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
  // fix seams betweencubemap faces
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}


void renderer_update()
{
	int w, h;
	window_get_size(&w, &h);
  // glViewport(0, 0, w, h);
 
  #ifdef EDITOR
  // -- opengl state --
  glEnable(GL_DEPTH_TEST); // enable the z-buffer
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND); // enable blending of transparent texture
  //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
  #endif

  // -- background --
	// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---- draw ----

  // @TODO: get this in advance
  int world_len = 0;
  int world_dead_len = 0;
  entity_t* world = state_get_entity_arr(&world_len, &world_dead_len);
  int dir_lights_len = 0;
  dir_light_t* dir_lights = state_get_dir_light_arr(&dir_lights_len);
  int point_lights_len = 0;
  point_light_t* point_lights = state_get_point_light_arr(&point_lights_len);
  

  TIMER_START("shadow maps");
  // shadow maps ------------------------------------------------------------
  
  mat4 proj; 
  mat4_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 12.5f, proj);  // shadow_near_plane, shadow_far_plane

  mat4 view;
  vec3 center = { 0.0f,  0.0f,  0.0f };
  vec3 up     = { 0.0f,  1.0f,  0.0f };


  // @TODO: shadows follow camera
  vec3 light_pos, c_forward;
  camera_get_pos(light_pos);
  vec3_add(light_pos, VEC3_XYZ(0, 5, 0), light_pos);
  camera_get_front(c_forward);
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
    }
    for (int i = 0; i < core_data->terrain_chunks_len; ++i) 
    { 
      if (!core_data->terrain_chunks[i].loaded || !core_data->terrain_chunks[i].visible) { continue; }
      shader_set_mat4(&core_data->shadow_shader, "model", core_data->terrain_chunks[i].model);
      renderer_draw_terrain_mesh(&core_data->terrain_chunks[i]); 
    }
    framebuffer_unbind();
  }
  core_data->show_shadows = shadow_caster_counter > 0 ? true : false;
  glViewport(0, 0, w, h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	{ glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else
	{ glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

  framebuffer_bind(&core_data->fb_deferred);
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
      glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->albedo)->handle); 
      
      glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->normal)->handle); 
      
      glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->roughness)->handle); 
      
      glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
      glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->metallic)->handle);
    
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
    }
    
    for (int i = 0; i < core_data->terrain_chunks_len; ++i) 
    { 
      if (!core_data->terrain_chunks[i].loaded || !core_data->terrain_chunks[i].visible) { continue; }
      renderer_draw_terrain(view, proj, &core_data->terrain_chunks[i]); 
    }
    
    // skybox -----------------------------------------------------------------
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

    shader_use(&core_data->skybox_shader);
    shader_set_mat4(&core_data->skybox_shader, "view", view_no_pos);
    shader_set_mat4(&core_data->skybox_shader, "proj", proj);

    // skybox cube
    glBindVertexArray(skybox_vao);
    glActiveTexture(GL_TEXTURE0);
    shader_set_int(&core_data->skybox_shader, "cube_map", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.environment);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
    // ------------------------------------------------------------------------
  }
  framebuffer_unbind();
  TIMER_STOP();

  TIMER_START("shadow pass");
  // shadow pass -----------------------------------------
  framebuffer_bind(&core_data->fb_shadow_pass);
  {    
    if (core_data->wireframe_mode_enabled == true)
    { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    shader_use(&core_data->shadow_pass_shader);
    
    int  tex_index = 0;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer03); 
    shader_set_int(&core_data->shadow_pass_shader, "normal", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer04); 
    shader_set_int(&core_data->shadow_pass_shader, "position", tex_index);
    tex_index++;

    // lights ----------------------------------------------
    // @UNSURE: limit shadow casters to 1 ??? 
    char buffer[28];
    shader_set_int(&core_data->shadow_pass_shader, "shadows_len", core_data->show_shadows ? 1 : 0);
    for (int i = 0; i < 1; ++i)
    {
      dir_light_t* light = &dir_lights[i];

      int idx = i; //  - disabled_lights;
      sprintf(buffer, "shadows[%d].direction", idx);
      shader_set_vec3(&core_data->shadow_pass_shader, buffer, light->dir);

      sprintf(buffer, "shadows[%d].shadow_map", idx);
      glActiveTexture(GL_TEXTURE0 + tex_index);
      glBindTexture(GL_TEXTURE_2D, light->fb_shadow.buffer);
      shader_set_int(&core_data->shadow_pass_shader, buffer, tex_index);
      tex_index++;
      sprintf(buffer, "shadows[%d].view", idx);
      shader_set_mat4(&core_data->shadow_pass_shader, buffer, light->view);
      sprintf(buffer, "shadows[%d].proj", idx);
      shader_set_mat4(&core_data->shadow_pass_shader, buffer, light->proj);
    
      ERR_CHECK(tex_index <= 31, "bound GL_TEXTURE%d, max: 31\n", tex_index);
    }
    // -----------------------------------------------------

    glBindVertexArray(core_data->quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
  }
  framebuffer_unbind();
  TIMER_STOP();

  TIMER_START("lighting");
  // lighting --------------------------------------------
  framebuffer_bind(&core_data->fb_lighting);
  {    
    if (core_data->wireframe_mode_enabled == true)
    { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    shader_use(&core_data->lighting_shader);
    
    vec3 cam_pos; camera_get_pos(cam_pos);
    shader_set_vec3(&core_data->lighting_shader, "view_pos", cam_pos);
    
    int  tex_index = 0;

    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.irradiance);
    shader_set_int(&core_data->lighting_shader, "irradiance_map", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, core_data->cube_map.prefilter);
    shader_set_int(&core_data->lighting_shader, "prefilter_map", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, brdf_lut);
    shader_set_int(&core_data->lighting_shader, "brdf_lut", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer);
    shader_set_int(&core_data->lighting_shader, "color", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer02);
    shader_set_int(&core_data->lighting_shader, "material", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer03); 
    shader_set_int(&core_data->lighting_shader, "normal", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index);
    glBindTexture(GL_TEXTURE_2D, core_data->fb_deferred.buffer04); 
    shader_set_int(&core_data->lighting_shader, "position", tex_index);
    tex_index++;
    glActiveTexture(GL_TEXTURE0 + tex_index); 
    glBindTexture(GL_TEXTURE_2D, core_data->fb_shadow_pass.buffer); 
    shader_set_int(&core_data->lighting_shader, "shadow", tex_index);
    tex_index++;

    ERR_CHECK(tex_index <= 31, "bound GL_TEXTURE%d, max: 31\n", tex_index);

    // lights ----------------------------------------------
    char buffer[28];
    // int  shadow_idx = 0;
    shader_set_int(&core_data->lighting_shader, "dir_lights_len", dir_lights_len);
    for (int i = 0; i < dir_lights_len; ++i)
    {
      dir_light_t* light = &dir_lights[i];

      int idx = i; //  - disabled_lights;
      sprintf(buffer, "dir_lights[%d].direction", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, light->dir);

      vec3 color;
      vec3_mul_f(light->color, light->intensity, color);
      sprintf(buffer, "dir_lights[%d].color", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, color);
    }
    shader_set_int(&core_data->lighting_shader, "point_lights_len", point_lights_len);
    for (int i = 0; i < point_lights_len; ++i)
    {
      point_light_t* light = &point_lights[i];

      int idx = i; //  - disabled_lights;
      sprintf(buffer, "point_lights[%d].pos", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, light->pos);
      sprintf(buffer, "point_lights[%d].color", idx);
      shader_set_vec3(&core_data->lighting_shader, buffer, light->color);
      sprintf(buffer, "point_lights[%d].intensity", idx);
      shader_set_float(&core_data->lighting_shader, buffer, light->intensity);
    }
    // -----------------------------------------------------

    glBindVertexArray(core_data->quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
  }
  framebuffer_unbind();
  TIMER_STOP();

#ifdef OUTLINE
  TIMER_FUNC(renderer_draw_scene_outline());
#endif // EDITOR

  TIMER_START("post fx");
  // post fx ---------------------------------------------

  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  shader_use(&core_data->post_fx_shader);
  shader_set_float(&core_data->post_fx_shader, "exposure", exposure);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, core_data->fb_lighting.buffer);  // fb_mouse_pick.buffer); // dir_lights[0].fb_shadow.buffer // core_data->fb_deferred.buffer03);
  shader_set_int(&core_data->post_fx_shader, "tex", 0);

#ifdef OUTLINE
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, core_data->fb_outline.buffer);  // fb_mouse_pick.buffer); // dir_lights[0].fb_shadow.buffer // core_data->fb_deferred.buffer03);
  shader_set_int(&core_data->post_fx_shader, "outline", 1);
#endif // OUTLINE

  glBindVertexArray(core_data->quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);

  // -----------------------------------------------------
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
  char buf[64];
  u32 tex_idx = 0;
  for (u32 i = 0; i < core_data->terrain_materials_len; ++i)
  {
    material_t* mat = assetm_get_material_by_idx(core_data->terrain_materials[i]);
    
    shader_use(&core_data->terrain_shader);
    
    sprintf(buf, "materials[%d].albedo", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->albedo)->handle); 
    
    sprintf(buf, "materials[%d].normal", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx); 
    glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->normal)->handle); 
    
    sprintf(buf, "materials[%d].roughness", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->roughness)->handle); 

    sprintf(buf, "materials[%d].metallic", i);
    shader_set_int(&core_data->terrain_shader, buf, tex_idx);
    glActiveTexture(GL_TEXTURE0 + tex_idx); tex_idx++;
    glBindTexture(GL_TEXTURE_2D, assetm_get_texture_by_idx(mat->metallic)->handle);

    sprintf(buf, "materials[%d].tint", i);
    shader_set_vec3(&core_data->terrain_shader, buf, mat->tint);

    sprintf(buf, "materials[%d].roughness_f", i);
    shader_set_float(&core_data->terrain_shader, buf, mat->roughness_f);
    sprintf(buf, "materials[%d].metallic_f", i);
    shader_set_float(&core_data->terrain_shader, buf, mat->metallic_f);
  }
  shader_set_mat4(&core_data->terrain_shader, "model", chunk->model);  // model gets updated in shadow map
  shader_set_mat4(&core_data->terrain_shader, "view", view);
  shader_set_mat4(&core_data->terrain_shader, "proj", proj);


  // draw mesh
  // glCullFace(GL_FRONT);
  glBindVertexArray(chunk->vao);
  // render the mesh triangle strip by triangle strip - each row at a time
  for(u32 strip = 0; strip < chunk->strips_num; ++strip)
  {
    glDrawElements(GL_TRIANGLE_STRIP,       // primitive type
        chunk->verts_per_strip,             // number of indices to render
        GL_UNSIGNED_INT,                    // index data type
        (void*)(sizeof(u32)
          * chunk->verts_per_strip
          * strip)); // offset to starting index
  }
  // glCullFace(GL_BACK);
}

void renderer_draw_terrain_mesh(terrain_chunk_t* chunk)
{
  // draw mesh
  glBindVertexArray(chunk->vao);
  // render the mesh triangle strip by triangle strip - each row at a time
  for(u32 strip = 0; strip < chunk->strips_num; ++strip)
  {
    glDrawElements(GL_TRIANGLE_STRIP,       // primitive type
        chunk->verts_per_strip,             // number of indices to render
        GL_UNSIGNED_INT,                    // index data type
        (void*)(sizeof(u32)
          * chunk->verts_per_strip
          * strip)); // offset to starting index
  }
}
#ifdef EDITOR
void renderer_draw_scene_mouse_pick(mat4 gizmo_model)
{
  int w, h; window_get_size(&w, &h);
  app_data_t*  app_data  = app_data_get();
  
  framebuffer_bind(&core_data->fb_mouse_pick);
  glViewport(0, 0, w / 4, h / 4);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // -- opengl state --
  glEnable(GL_DEPTH_TEST); // enable the z-buffer
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  mat4 view, proj;
  camera_get_view_mat(view);
  camera_get_proj_mat(w, h, proj);

  // cycle all objects
  int entities_len = 0;
  int entities_dead_len = 0;
  entity_t* entities = state_get_entity_arr(&entities_len, &entities_dead_len);
  shader_use(&core_data->mouse_pick_shader);
  for (int i = 0; i < entities_len; ++i)
  {
    entity_t* ent = &entities[i];
    if (ent->is_dead || ent->mesh < 0 || ent->mat < 0) { continue; }

    // state_entity_update_global_model(i);

    shader_set_float(&core_data->mouse_pick_shader, "id", (f32)ent->id);

    shader_set_mat4(&core_data->mouse_pick_shader, "model", ent->model);
    shader_set_mat4(&core_data->mouse_pick_shader, "view", view);
    shader_set_mat4(&core_data->mouse_pick_shader, "proj", proj);

    mesh_t* mesh = assetm_get_mesh_by_idx(ent->mesh);
    DRAW_MESH(mesh);
  }

  // -- draw terrain --
  for (int i = 0; i < core_data->terrain_chunks_len; ++i) 
  { 
  if (!core_data->terrain_chunks[i].loaded || !core_data->terrain_chunks[i].visible) { continue; }
    shader_set_float(&core_data->mouse_pick_shader, "id", (f32)ID_BUFFER_TERRAIN_0 -i); // counts down
    shader_set_mat4(&core_data->mouse_pick_shader, "model", core_data->terrain_chunks[i].model);
    renderer_draw_terrain_mesh(&core_data->terrain_chunks[i]); 
  }

  // -- draw lights --
  mesh_t* sphere = assetm_get_mesh("sphere.fbx");
  for (int i = 0; i < entities_len; ++i)
  {
    entity_t* ent = &entities[i];
    if (ent->point_light_idx >= 0)
    {
      mat4 model;
      mat4_copy(ent->model, model);
      mat4_scale_f(model, 0.35f, model);
      shader_set_float(&core_data->mouse_pick_shader, "id", (f32)ent->id);

      shader_set_mat4(&core_data->mouse_pick_shader, "model", model);
      shader_set_mat4(&core_data->mouse_pick_shader, "view", view);
      shader_set_mat4(&core_data->mouse_pick_shader, "proj", proj);

      DRAW_MESH(sphere);
    }
  }

  // -- draw gizmo --
  if (app_data->selected_id >= 0 || app_data->selected_id == -2) // entity or terrain
  {
    glClear(GL_DEPTH_BUFFER_BIT);
        
    // mat4 model;
    // if (app_data->selected_id >= 0)
    // { state_entity_model_no_scale(app_data->selected_id, model); }
    // else // terrain
    // { mat4_make_model(core_data->terrain_pos, core_data->terrain_rot, VEC3(1), model); }


    shader_set_mat4(&core_data->mouse_pick_shader, "model", gizmo_model);
    shader_set_mat4(&core_data->mouse_pick_shader, "view", view);
    shader_set_mat4(&core_data->mouse_pick_shader, "proj", proj);

    mesh_t* hitboxes_translate[] = 
    {
      assetm_get_mesh("gizmos/translate/x.fbx"),
      assetm_get_mesh("gizmos/translate/y.fbx"),
      assetm_get_mesh("gizmos/translate/z.fbx"),
      assetm_get_mesh("gizmos/translate/xy.fbx"),
      assetm_get_mesh("gizmos/translate/xz.fbx"),
      assetm_get_mesh("gizmos/translate/yz.fbx"),
      assetm_get_mesh("gizmos/translate/xyz.fbx")
    };
    mesh_t* hitboxes_scale[] = 
    {
      assetm_get_mesh("gizmos/scale/x.fbx"),
      assetm_get_mesh("gizmos/scale/y.fbx"),
      assetm_get_mesh("gizmos/scale/z.fbx"),
      assetm_get_mesh("gizmos/scale/xyz.fbx")
    };
    mesh_t* hitboxes_rotate[] = 
    {
      assetm_get_mesh("gizmos/rotate/x.fbx"),
      assetm_get_mesh("gizmos/rotate/y.fbx"),
      assetm_get_mesh("gizmos/rotate/z.fbx"),
      assetm_get_mesh("gizmos/rotate/xyz.fbx")
    };

    mesh_t** hitboxes = app_data->gizmo_type == 1 ? hitboxes_translate :
      app_data->gizmo_type == 2 ? hitboxes_scale     : 
      app_data->gizmo_type == 3 ? hitboxes_rotate    : NULL;
    int hitboxes_len  = app_data->gizmo_type == 1 ? 7 :
      app_data->gizmo_type == 2 ? 4 :
      app_data->gizmo_type == 3 ? 4 : 0;

    for (int i = 0; i < hitboxes_len; ++i)
    {
      shader_set_float(&core_data->mouse_pick_shader, "id", (f32)ID_BUFFER_GIZMO_0 -i); // -3, -4, -5, ...
      mesh_t* mesh = hitboxes[i];
      DRAW_MESH(mesh);
    }
  }

  framebuffer_unbind();
}

void renderer_draw_scene_outline()
{
  if (core_data->outline_id < 0) { return; }
  bool error = false;
	entity_t* e = state_get_entity(core_data->outline_id, &error); ASSERT(!error);
  // draw in solid-mode for fbo
	if (core_data->wireframe_mode_enabled == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);
	int w, h; window_get_size(&w, &h);
	glViewport(0, 0, w, h);
	framebuffer_bind(&core_data->fb_outline);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg
	
  // if (!(gamestate && hide_gizmos) && ent->id != -1 && (ent->has_model || ent->has_light || ent->has_cam)) //  && ent->visible
	// {
  // }

  // @TODO: draw lightbulb when light

  // mat4 view, proj;
  // camera_get_view_mat(view);
  // mat4 view_no_pos;
  // mat4_copy(view, view_no_pos);
  // mat4_set_pos(0, 0, 0, view_no_pos);
  // camera_get_proj_mat(w, h, proj);

  // shader_set_mat4(&core_data->_shader, "model", ent->model);
  // shader_set_mat4(&core_data->_shader, "view", view);
  // shader_set_mat4(&core_data->_shader, "proj", proj);


  // @NOTE: does obj drawn need to be scaled up ?

  mesh_t* mesh   = assetm_get_mesh_by_idx(e->mesh);
  texture_t* tex = assetm_get_texture("#internal/blank.png", true);
  renderer_direct_draw_mesh_textured_mat(e->model, mesh, tex, RGB_F_RGB(1));
  // DRAW_MESH(mesh);

	framebuffer_unbind();
}

#endif // EDITOR

int renderer_mouse_position_mouse_pick_id()
{
  f64 x = 0;
  f64 y = 0;
  input_get_mouse_pos(&x, &y);
  int w = 0;
  int h = 0;
  window_get_size(&w, &h);
  y = h - y; // invert as buffer is rendered upside down
  x *= 0.25;
  y *= 0.25;

  f32 pixel[1];

  glGetError(); // clear any previous errors
  glBindFramebuffer(GL_READ_FRAMEBUFFER, core_data->fb_mouse_pick.fbo);
  glReadPixels((int)x, (int)y, 1, 1, GL_RED, GL_FLOAT, pixel);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
 
  framebuffer_unbind();

  int id = pixel[0] -1;
  // PF("-> id: %d, pixel: %f\n", id, pixel[0]);

  return id;

}

u32 renderer_gen_brdf_lut()
{
  // gen framebuffer ---------------------------------------------------------------------

  unsigned int capture_fbo, capture_rbo;
  glGenFramebuffers(1, &capture_fbo);
  glGenRenderbuffers(1, &capture_rbo);

  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_fbo);

  // gen brdf lut ------------------------------------------------------------------------
  
  unsigned int brdf_lut;
  glGenTextures(1, &brdf_lut);

  glBindTexture(GL_TEXTURE_2D, brdf_lut);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_lut, 0);

  glViewport(0, 0, 512, 512);
  shader_use(&core_data->brdf_lut_shader);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	glBindVertexArray(core_data->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
  glDeleteFramebuffers(1, &capture_fbo);
  glDeleteRenderbuffers(1, &capture_rbo);

  return brdf_lut;
}


