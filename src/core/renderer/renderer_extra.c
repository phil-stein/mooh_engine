#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"
#include "core/renderer/renderer_extra.h"
#include "core/core_data.h"
#include "core/window.h"
#include "core/camera.h"
#include "core/state.h"
#include "core/io/assetm.h"
#include "core/io/asset_io.h"
#include "core/io/file_io.h"
#include "core/debug/debug_opengl.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"

#include "editor/app.h"   // @NOTE: big bad only tmp

static core_data_t* core_data = NULL;

void renderer_extra_init()
{
  core_data = core_data_get();
}

#ifdef EDITOR
void renderer_extra_draw_scene_mouse_pick(mat4 gizmo_model)
{
  int w, h; window_get_size(&w, &h);
  app_data_t*  app_data  = app_data_get();  // @NOTE: fucks this gargage
  
  framebuffer_bind(&core_data->fb_mouse_pick);
  _glViewport(0, 0, w / 4, h / 4);
  _glClearColor(0.0, 0.0, 0.0, 1.0);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // -- opengl state --
  _glEnable(GL_DEPTH_TEST); // enable the z-buffer
  _glEnable(GL_CULL_FACE);
  _glCullFace(GL_BACK);

  mat4 view, proj;
  camera_get_view_mat(view);
  camera_get_proj_mat(w, h, proj);

  // cycle all objects
  int entities_len = 0;
  int entities_dead_len = 0;
  entity_t* entities = state_entity_get_arr(&entities_len, &entities_dead_len);
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
  mesh_t* sphere = assetm_get_mesh("sphere");
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
    _glClear(GL_DEPTH_BUFFER_BIT);
        
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
      assetm_get_mesh("gizmos/translate/x"),
      assetm_get_mesh("gizmos/translate/y"),
      assetm_get_mesh("gizmos/translate/z"),
      assetm_get_mesh("gizmos/translate/xy"),
      assetm_get_mesh("gizmos/translate/xz"),
      assetm_get_mesh("gizmos/translate/yz"),
      assetm_get_mesh("gizmos/translate/xyz")
    };
    mesh_t* hitboxes_scale[] = 
    {
      assetm_get_mesh("gizmos/scale/x"),
      assetm_get_mesh("gizmos/scale/y"),
      assetm_get_mesh("gizmos/scale/z"),
      assetm_get_mesh("gizmos/scale/xyz")
    };
    mesh_t* hitboxes_rotate[] = 
    {
      assetm_get_mesh("gizmos/rotate/x"),
      assetm_get_mesh("gizmos/rotate/y"),
      assetm_get_mesh("gizmos/rotate/z"),
      assetm_get_mesh("gizmos/rotate/xyz")
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

void renderer_extra_draw_scene_outline()
{
  // @OPTIMIZATION: only clear buffer when deselecting
  _glClearColor(0.0, 0.0, 0.0, 0.0);
  int w, h; window_get_size(&w, &h);
  _glViewport(0, 0, w, h);
  framebuffer_bind(&core_data->fb_outline);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg
  
  if (core_data->outline_id < 0)  // no entity to render outline for 
  {
	  framebuffer_unbind();
    return; 
  }  

  // draw in solid-mode for fbo
	if (core_data->wireframe_mode_enabled == true)
	{ _glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
  
	entity_t* e = state_entity_get(core_data->outline_id);
  
  mesh_t* mesh = NULL;
  texture_t* tex = assetm_get_texture("#internal/blank.png", true);
  
  if (e->point_light_idx >= 0 && e->mesh < 0)                    // if no mesh, but is pointlight
  { 
    mesh = assetm_get_mesh("gizmos/lightbulb"); 
    // @TODO: rot & scl should be the macro in editor/stylesheet.h, buts thats a dependency
    renderer_direct_draw_mesh_textured(e->pos, VEC3_XYZ(90, 0, 0), VEC3(2.35f), mesh, tex, RGB_F_RGB(1));
  }           
  else if (e->is_dead || e->mesh < 0 || e->mat < 0) { framebuffer_unbind(); return; } // if no mesh
  else                                                          // if has mesh 
  {
    mesh = assetm_get_mesh_by_idx(e->mesh); // [m]
    renderer_direct_draw_mesh_textured_mat(e->model, mesh, tex, RGB_F_RGB(1));
  }              
	
	framebuffer_unbind();
}

#endif // EDITOR

int renderer_extra_mouse_position_mouse_pick_id()
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

  _glBindFramebuffer(GL_READ_FRAMEBUFFER, core_data->fb_mouse_pick.fbo);
  _glReadPixels((int)x, (int)y, 1, 1, GL_RED, GL_FLOAT, pixel);
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
 
  framebuffer_unbind();

  int id = pixel[0] -1;
  // PF("-> id: %d, pixel: %f\n", id, pixel[0]);

  return id;

}

u32 renderer_extra_gen_brdf_lut(const char* path)
{
  const int width  = 512;
  const int height = 512;

  // gen framebuffer ---------------------------------------------------------------------

  u32 capture_fbo, capture_rbo;
  _glGenFramebuffers(1, &capture_fbo);
  _glGenRenderbuffers(1, &capture_rbo);

  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  _glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  _glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  _glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_fbo);

  // gen brdf lut ------------------------------------------------------------------------
  
  u32 brdf_lut;
  _glGenTextures(1, &brdf_lut);

  _glBindTexture(GL_TEXTURE_2D, brdf_lut);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  _glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
  _glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
  _glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_lut, 0);

  _glViewport(0, 0, width, height);
  shader_use(&core_data->brdf_lut_shader);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	_glBindVertexArray(core_data->quad_vao);
	_glDrawArrays(GL_TRIANGLES, 0, 6);
  _glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _glDeleteFramebuffers(1, &capture_fbo);
  _glDeleteRenderbuffers(1, &capture_rbo);

  const int channel_nr = 2;

  u32 pixels_len = 0;
  texture_t t;
  t.handle     = brdf_lut;
  t.width      = width;
  t.height     = height;
  t.channel_nr = channel_nr;
  asset_io_texture_write_pixels_to_file(&t,  GL_RG, path);

  pixels_len++; // so gcc doesnt complain about unused variable

    
  return brdf_lut;
}





