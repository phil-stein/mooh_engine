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
  // -- assetm --

  core_data.equirect_shader = shader_create_from_file(ASSET_PATH"shaders/cubemap/render_equirect.vert", 
					      ASSET_PATH"shaders/cubemap/render_equirect.frag", NULL, "equirect_render_shader");
 
  core_data.irradiance_map_shader = shader_create_from_file(ASSET_PATH"shaders/cubemap/render_equirect.vert", 
					      ASSET_PATH"shaders/cubemap/irradiance_map.frag", NULL, "irradiance_map_shader");
 
  core_data.prefilter_shader = shader_create_from_file(ASSET_PATH"shaders/cubemap/render_equirect.vert", 
					      ASSET_PATH"shaders/cubemap/prefilter_map.frag", NULL, "prefilter_shader");


  core_data_init_renderer();

  // -- terrain --

  core_data.terrain_shader = assetm_create_shader_from_template(SHADER_TEMPLATE_TERRAIN);
}

void core_data_play()
{
  if (core_data_is_play()) { return; }
  
  core_data.scripts_act = true;
  core_data.phys_act    = true;

  serialization_write_scene_to_state_buffer();
}

void core_data_play_scripts()
{
  if (core_data_is_play()) { return; }

  core_data.scripts_act = true;
  core_data.phys_act    = false;

  serialization_write_scene_to_state_buffer();
}

void core_data_play_phys()
{
  if (core_data_is_play()) { return; }
  
  core_data.scripts_act = false;
  core_data.phys_act    = true;

  serialization_write_scene_to_state_buffer();
}

void core_data_pause()
{
  if (!core_data_is_play()) { return; }

  core_data.scripts_act = false;
  core_data.phys_act    = false;

  serialization_load_scene_from_state_buffer();
}

bool core_data_is_play()
{
  return core_data.phys_act || core_data.scripts_act;
}

INLINE void core_data_init_renderer()
{
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
