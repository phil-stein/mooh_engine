#include "core/core_data.h"
#include "core/assetm.h"
#include "core/window.h"
#include "core/serialization.h"



// -- func decls --
INLINE void core_data_init_renderer();


// ---- vars ----
core_data_t core_data = CORE_DATA_INIT();

core_data_t* core_data_get()
{
  return &core_data;
}

void core_data_init()
{
  // @NOTE: asset_path gets set in program_start()

  // -- assetm --

  char vert_path[ASSET_PATH_MAX + 64];
  char frag_path[ASSET_PATH_MAX + 64];
  sprintf(vert_path, "%sshaders/cubemap/render_equirect.vert", core_data.asset_path);
  sprintf(frag_path, "%sshaders/cubemap/render_equirect.frag", core_data.asset_path);
  core_data.equirect_shader = shader_create_from_file(vert_path, frag_path, NULL, "equirect_render_shader");
 
  sprintf(frag_path, "%sshaders/cubemap/irradiance_map.frag", core_data.asset_path);
  core_data.irradiance_map_shader = shader_create_from_file(vert_path, 
					      frag_path, NULL, "irradiance_map_shader");
 
  sprintf(frag_path, "%sshaders/cubemap/prefilter_map.frag", core_data.asset_path);
  core_data.prefilter_shader = shader_create_from_file(vert_path, 
					      frag_path, NULL, "prefilter_shader");


  core_data_init_renderer();

  // -- terrain --

  core_data.terrain_shader = assetm_create_shader_from_template(SHADER_TEMPLATE_TERRAIN);
}

void core_data_play()
{
  if (core_data_is_play()) { return; }
  
  core_data.scripts_act = true;
  core_data.phys_act    = true;

#if EDITOR
  serialization_write_scene_to_state_buffer();
#endif
}

void core_data_play_scripts()
{
  if (core_data_is_play()) { return; }

  core_data.scripts_act = true;
  core_data.phys_act    = false;

#if EDITOR
  serialization_write_scene_to_state_buffer();
#endif
}

void core_data_play_phys()
{
  if (core_data_is_play()) { return; }
  
  core_data.scripts_act = false;
  core_data.phys_act    = true;

#if EDITOR
  serialization_write_scene_to_state_buffer();
#endif
}

void core_data_pause()
{
  if (!core_data_is_play()) { return; }

  core_data.scripts_act = false;
  core_data.phys_act    = false;

#if EDITOR
  serialization_load_scene_from_state_buffer();
#endif
}

bool core_data_is_play()
{
  return core_data.phys_act || core_data.scripts_act;
}

INLINE void core_data_init_renderer()
{
  // -- primitives --
  	
  // screen quad 
	f32 quad_verts[] = 
	{ 
	// positions   // tex coords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &core_data.quad_vao);
	glGenBuffers(1, &core_data.quad_vbo);
	glBindVertexArray(core_data.quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, core_data.quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(f32), &quad_verts, GL_STATIC_DRAW); // quad_verts is 24 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));

  // quad
  core_data.quad_mesh = assetm_get_mesh_idx("quad.fbx");
 
  // line
  const f32 verts[] = 
  {
    // pos    normals   uvs    tangents
    0, 0, 0,  0, 0, 0,  0, 0,  0, 0, 0, 
    0, 1, 0,  0, 0, 0,  0, 0,  0, 0, 0, 
  };
  const int verts_len = 2 * FLOATS_PER_VERT;
  mesh_make((f32*)verts, (int)verts_len, &core_data.line_mesh);

  // -- framebuffers --
  core_data.fb_deferred.type = FRAMEBUFFER_DEFERRED;
  core_data.fb_deferred.is_msaa = false;
  core_data.fb_deferred.width  = 0;
  core_data.fb_deferred.height = 0;
  core_data.fb_deferred.size_divisor = 1;
  framebuffer_create(&core_data.fb_deferred);
  window_set_texturebuffer_to_update_to_screen_size(&core_data.fb_deferred);
   
  core_data.fb_shadow_pass.type = FRAMEBUFFER_SINGLE_CHANNEL_F;
  core_data.fb_shadow_pass.is_msaa = false;
  core_data.fb_shadow_pass.width  = 0;
  core_data.fb_shadow_pass.height = 0;
  core_data.fb_shadow_pass.size_divisor = 1;
  framebuffer_create(&core_data.fb_shadow_pass);
  window_set_texturebuffer_to_update_to_screen_size(&core_data.fb_shadow_pass);  

  core_data.fb_lighting.type = FRAMEBUFFER_RGB16F;
  core_data.fb_lighting.is_msaa = false;
  core_data.fb_lighting.width  = 0;
  core_data.fb_lighting.height = 0;
  core_data.fb_lighting.size_divisor = 1;
  framebuffer_create(&core_data.fb_lighting);
  window_set_texturebuffer_to_update_to_screen_size(&core_data.fb_lighting);
 
  core_data.fb_mouse_pick.type    = FRAMEBUFFER_SINGLE_CHANNEL_F;
  core_data.fb_mouse_pick.is_msaa = false;
  core_data.fb_mouse_pick.width   = 0;
  core_data.fb_mouse_pick.height  = 0;
  core_data.fb_mouse_pick.size_divisor = 4;
  framebuffer_create(&core_data.fb_mouse_pick);
  window_set_texturebuffer_to_update_to_screen_size(&core_data.fb_mouse_pick);

  // -- shaders --

  core_data.basic_shader       = assetm_create_shader_from_template(SHADER_TEMPLATE_BASIC);
  
  core_data.shadow_shader      = assetm_create_shader_from_template(SHADER_TEMPLATE_SHADOW_MAP);

  core_data.deferred_shader    = assetm_create_shader_from_template(SHADER_TEMPLATE_DEFERRED);
	
  core_data.skybox_shader      = assetm_create_shader_from_template(SHADER_TEMPLATE_SKYBOX);
  
  core_data.shadow_pass_shader = assetm_create_shader_from_template(SHADER_TEMPLATE_SHADOW_PASS);
  
  core_data.lighting_shader    = assetm_create_shader_from_template(SHADER_TEMPLATE_LIGHTING);

  core_data.post_fx_shader     = assetm_create_shader_from_template(SHADER_TEMPLATE_POST_FX);

  core_data.brdf_lut_shader    = assetm_create_shader_from_template(SHADER_TEMPLATE_BRDF_LUT);

  core_data.mouse_pick_shader  = assetm_create_shader_from_template(SHADER_TEMPLATE_MOUSE_PICK);
}
