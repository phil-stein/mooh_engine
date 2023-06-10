#include "core/renderer/renderer_direct.h"
#include "core/core_data.h"
#include "core/io/assetm.h"
#include "core/window.h"
#include "core/camera.h"
#include "core/debug/debug_opengl.h"

static core_data_t* core_data;




void renderer_direct_init()
{
  core_data = core_data_get();
}

// @TODO: doesnt work, used in mui.c
void renderer_direct_draw_quad(vec2 cam_pos, f32 cam_zoom, vec2 pos, vec2 size, vec3 color)
{
  renderer_direct_draw_quad_textured(cam_pos, cam_zoom, pos, size, assetm_get_texture("#internal/blank.png", true), color);
}

void renderer_direct_draw_quad_textured(vec2 cam_pos, f32 cam_zoom, vec2 pos, vec2 size, texture_t* tex, rgbf tint)
{
  // ---- mvp ----
	
	mat4 model;
	mat4_make_model_2d(pos, size, 0.0f, model);

	mat4 view;
	mat4_lookat_2d(cam_pos, cam_zoom, view);

	int w, h;
	window_get_size(&w, &h);
	mat4 proj;
  camera_get_proj_mat(w, h, proj);

	// ---- shader & draw call -----	

	shader_use(&core_data->basic_shader);
	shader_set_vec3(&core_data->basic_shader, "tint", tint);
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, tex->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

	// glBindVertexArray(core_data->quad_vao);
	// glDrawArrays(GL_TRIANGLES, 0, 6);
	
  mesh_t* m = assetm_get_mesh_by_idx(core_data->quad_mesh);
	glBindVertexArray(m->vao);
  if (m->indexed)
  { _glDrawElements(GL_TRIANGLES, m->indices_count, GL_UNSIGNED_INT, 0); }
  else
  { _glDrawArrays(GL_TRIANGLES, 0, m->verts_count); }
	
}

void renderer_direct_draw_mesh_textured(vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint)
{
	// ---- mvp ----
	mat4 model;
  mat4_make_model(pos, rot, scale, model);

	mat4 view;
  // vec3 center;
  // cam_pos[2] = 10;  // tmp, zoom away
  // vec3_add(cam_pos, VEC3_Z(-1), center);
  // mat4_lookat(cam_pos, center, VEC3_Y(1), view);
  camera_get_view_mat(view);
  

	int w, h;
	window_get_size(&w, &h);
	mat4 proj;
  camera_get_proj_mat(w, h, proj);

	// ---- shader & draw call -----	

	shader_use(&core_data->basic_shader);
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, tex->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	shader_set_vec3(&core_data->basic_shader, "tint", tint);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

	_glBindVertexArray(m->vao);
  if (m->indexed)
  { _glDrawElements(GL_TRIANGLES, m->indices_count, GL_UNSIGNED_INT, 0); }
  else
  { _glDrawArrays(GL_TRIANGLES, 0, m->verts_count); }
	
}

void renderer_direct_draw_mesh_textured_mat(mat4 model, mesh_t* m, texture_t* tex, vec3 tint)
{
	// ---- mvp ----
	mat4 view;
  camera_get_view_mat(view);
  

	int w, h;
	window_get_size(&w, &h);
	mat4 proj;
  camera_get_proj_mat(w, h, proj);

	// ---- shader & draw call -----	

	shader_use(&core_data->basic_shader);
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, tex->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	shader_set_vec3(&core_data->basic_shader, "tint", tint);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

	_glBindVertexArray(m->vao);
  if (m->indexed)
  { _glDrawElements(GL_TRIANGLES, m->indices_count, GL_UNSIGNED_INT, 0); }
  else
  { _glDrawArrays(GL_TRIANGLES, 0, m->verts_count); }
	
}


void renderer_direct_draw_mesh_preview(vec3 cam_pos, vec3 pos, vec3 rot, vec3 scale, mesh_t* m, texture_t* tex, vec3 tint, texture_t* bg, framebuffer_t* fb)
{
	framebuffer_bind(fb);
   
  _glViewport(0, 0, fb->width, fb->height);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  // -- opengl state --
  _glDisable(GL_DEPTH_TEST);
  _glEnable(GL_CULL_FACE);
  _glCullFace(GL_BACK);
  _glEnable(GL_BLEND); // enable blending of transparent texture
  //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
  _glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  _glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
 
  // -- background --
 	
  mat4 model;
  mat4_make_model(VEC3(0), VEC3(0), VEC3(1), model);

	mat4 view;
  vec3 center;
  vec3_add(VEC3_Z(10), VEC3_Z(-1), center);
  mat4_lookat(VEC3_Z(10), center, VEC3_Y(1), view);
  
	mat4 proj;
	float pers = 10;
	m_deg_to_rad(&pers);
	mat4_perspective(pers, ((f32)fb->width / (f32)fb->height), 0.1f, 100.0f, proj);

	shader_use(&core_data->basic_shader);
	shader_set_vec3(&core_data->basic_shader, "tint", VEC3(1));
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, bg->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

  mesh_t* quad_m = assetm_get_mesh_by_idx(core_data->quad_mesh);
	_glBindVertexArray(quad_m->vao);
  if (quad_m->indexed)
  { _glDrawElements(GL_TRIANGLES, quad_m->indices_count, GL_UNSIGNED_INT, 0); }
  else
  { _glDrawArrays(GL_TRIANGLES, 0, quad_m->verts_count); }

  glEnable(GL_DEPTH_TEST);
  
  // -- mesh --

  mat4_make_model(pos, rot, scale, model);

  vec3_add(cam_pos, VEC3_Z(-1), center);
  mat4_lookat(cam_pos, center, VEC3_Y(1), view);
  
	mat4_perspective(pers, ((f32)fb->width / (f32)fb->height), 0.1f, 100.0f, proj);

	shader_use(&core_data->basic_shader);
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, tex->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	shader_set_vec3(&core_data->basic_shader, "tint", tint);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

	_glBindVertexArray(m->vao);
  if (m->indexed)
  { _glDrawElements(GL_TRIANGLES, m->indices_count, GL_UNSIGNED_INT, 0); }
  else
  { _glDrawArrays(GL_TRIANGLES, 0, m->verts_count); }

  framebuffer_unbind();
	
}
void renderer_direct_draw_line(vec3 pos0, vec3 pos1, vec3 tint, f32 width)
{
	// ---- mvp ----
	mat4 model;
  mat4_make_model(VEC3(0), VEC3(0), VEC3(1), model);

	mat4 view;
  camera_get_view_mat(view);
  
	int w, h;
	window_get_size(&w, &h);
	mat4 proj;
  camera_get_proj_mat(w, h, proj);

  _glLineWidth(width);

  // ---- vbo sub data ----

  _glBindBuffer(GL_ARRAY_BUFFER, core_data->line_mesh.vbo);
  _glBufferSubData(GL_ARRAY_BUFFER, 0 * sizeof(f32),               3 * sizeof(f32), pos0);
  _glBufferSubData(GL_ARRAY_BUFFER, FLOATS_PER_VERT * sizeof(f32), 3 * sizeof(f32), pos1);

	// ---- shader & draw call -----	

	shader_use(&core_data->basic_shader);
	_glActiveTexture(GL_TEXTURE0);
	_glBindTexture(GL_TEXTURE_2D, (assetm_get_texture("#internal/blank.png", true))->handle); 
	shader_set_int(&core_data->basic_shader, "tex", 0);
	shader_set_vec3(&core_data->basic_shader, "tint", tint);
	
	shader_set_mat4(&core_data->basic_shader, "model", model);
	shader_set_mat4(&core_data->basic_shader, "view", view);
	shader_set_mat4(&core_data->basic_shader, "proj", proj);

	_glBindVertexArray(core_data->line_mesh.vao);
  _glDrawArrays(GL_LINES, 0, 2);
	
}
