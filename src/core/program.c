#include "core/program.h"
#include "core/core_data.h"
#include "core/camera.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"
#include "core/renderer/renderer_extra.h"
#include "core/input.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/io/asset_io.h"
#include "core/state.h"
#include "core/threadm.h"
#include "core/terrain.h"
#include "core/event_sys.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "serialization/serialization.h"
#include "math/math_inc.h"
#include "phys/phys_world.h"
#include "phys/phys_debug_draw.h"
#include "mui/mui.h"

// order is important, io_util & str_util before global
#define IO_UTIL_IMPLEMENTATION  // only define once
#include "global/io_util.h"     // only need to include here, normally included via global.h
#define STR_UTIL_IMPLEMENTATION // only define once
#include "global/str_util.h"    // only need to include here, normally included via global.h
#include "global/global.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"

#include <time.h>

// @TODO: put stb_image here to, its in texture.c i think
#define STB_DS_IMPLEMENTATION       // only define once
#include "stb/stb_ds.h"         


static core_data_t* core_data = NULL;

char  _title[WINDOW_TITLE_MAX];      // copy title
char __title[WINDOW_TITLE_MAX +14];  // copy _title, add fps


void DBG(program_start_dbg, int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f, const char* asset_path)
{
  TIMER_START(" -- program init -- ");

  if (!window_create(width, height, title, type))
	{
		printf("[ERROR] window creation failed\n");
		return;
	}

	// ---- init ----
  debug_timer_init();

  // asset path
  core_data = core_data_get();
  strcpy(core_data->asset_path, asset_path);
  SPRINTF(SHADERS_PATH_MAX, core_data->shaders_path, "%sshaders/", asset_path);
  core_data->use_async_asset_arrs = true; // use multithreaded asset loading 

  rand_seed(time(NULL));
	TIMER_FUNC_STATIC(input_init());
  TIMER_FUNC_STATIC(asset_io_init());
  TIMER_FUNC_STATIC(assetm_init());
  TIMER_FUNC_STATIC(threadm_init());

  // -- brdf_lut -> just take https://learnopengl.com/img/pbr/ibl_brdf_lut.png and convert
  // -- last two lines do that
  // // // gen brdf_lut, only needs to be done once, is loaded from textures/#internal/brdf_lut.tex in core_data_init
  // // char _path[ASSET_PATH_MAX + 256];
  // // SPRINTF(ASSET_PATH_MAX + 256, _path, "%s/textures/#internal/brdf_lut.tex", core_data->asset_path);
  // // P("pre render");
  // // TIMER_FUNC_STATIC(renderer_extra_gen_brdf_lut(_path)); 
  // asset_io_convert_texture("#internal/brdf_lut.png");
  // core_data->brdf_lut = assetm_get_texture("#internal/brdf_lut.tex", false)->handle; 

  TIMER_FUNC_STATIC(core_data_init());
  TIMER_FUNC_STATIC(state_init());
  TIMER_FUNC_STATIC(serialization_init());
	TIMER_FUNC_STATIC(save_sys_init());
  TIMER_FUNC_STATIC(renderer_direct_init());
  TIMER_FUNC_STATIC(renderer_extra_init());

  TIMER_FUNC_STATIC(camera_init());
  TIMER_FUNC_STATIC(renderer_init());
  TIMER_FUNC_STATIC(terrain_init());
  TIMER_FUNC_STATIC(mui_init());

	TIMER_FUNC_STATIC(init_f());     // init callback
  
  TIMER_FUNC_STATIC(debug_draw_init());
  
  TIMER_FUNC_STATIC(phys_init(event_sys_trigger_phys_collision, event_sys_trigger_phys_trigger)); 

  // @NOTE: entity->init() get called in the editor they get called when play is pressed
  #ifndef EDITOR
  TIMER_FUNC_STATIC(state_call_entity_init());
  #endif

  // @UNSURE: 
  //        - archive meshes and load that way
  //        - check if even faster
  u32* tex_arr_len_ptr = 0;
  texture_load_data_t** tex_arr_ptr = assetm_get_texture_register_arr_ptr(&tex_arr_len_ptr);
  TIMER_FUNC_STATIC(threadm_load_texture_arr(tex_arr_ptr, tex_arr_len_ptr));
  core_data->use_async_asset_arrs = false; // no multithreaded,  just normal asset loading 
 
  strcpy(_title, window_get_title()); 
  
  TIMER_STOP_STATIC();  // program init timer

  P_U32((u32)sizeof(entity_t));
  
  bool first_frame = true;
	while (!core_data->program_quit && !glfwWindowShouldClose(core_data->window))
	{
		// glfwRequestWindowAttention(window);
		glfwPollEvents();
		
		// ---- time ----
		core_data->delta_t      = (f32)glfwGetTime() - core_data->t_last_frame;
		core_data->t_last_frame = (f32)glfwGetTime();
    core_data->cur_fps      = 1 / core_data->delta_t;
    if (first_frame) { core_data->delta_t = 0.016f; first_frame = false; } // otherwise dt first frame is like 5 seconds
  
    // // --- title ----
    SPRINTF(WINDOW_TITLE_MAX, __title, "%s | fps: '%.1f'", _title, core_data->cur_fps);
    window_set_title(__title);

    // ---- update ----
    TIMER_FUNC(terrain_update());
    
    TIMER_FUNC(renderer_update());
    TIMER_FUNC(update_f());   // update callback
    
    TIMER_FUNC(state_update());
#ifdef EDITOR
    if (core_data->phys_act)
    {
#endif
      TIMER_FUNC(phys_update(core_data->delta_t));
      TIMER_FUNC(program_sync_phys());
#ifdef EDITOR
    }
#endif
    
    TIMER_FUNC(debug_draw_update());
		TIMER_FUNC(mui_update());
		TIMER_FUNC(input_update());
 
    debug_timer_clear_state();

		glfwSwapBuffers(core_data->window);
	}

  assetm_cleanup();

}

// @TODO: move this to state.c
// sync physics & entities
void program_sync_phys()
{
  // @TODO: if static parented to moving parent, or give option for dynamic objs to not have forces affect them i.e. push, gravity

  int world_len = 0;
  int world_dead_len = 0;
  entity_t* world   = state_entity_get_arr(&world_len, &world_dead_len);
  u32 phys_objs_len = 0;
  phys_obj_t* phys_objs = phys_get_obj_arr(&phys_objs_len);

  // @TMP: -------------------
  
  vec3 sum_aabb[2];
  vec3 b0[2]; bool b0_found = false; 
  vec3 b1[2]; bool b1_found = false;
  
  for (u32 i = 0; i < phys_objs_len; ++i)
  {
    phys_obj_t* obj = &phys_objs[i]; 
    if (obj->entity_idx == 0)
    { 
      // vec3_copy(obj->collider.box.aabb[0], b0[1]);
      // vec3_copy(obj->collider.box.aabb[1], b0[1]);
      phys_get_final_aabb(obj, b0);

      b0_found = true;
    }
    if (obj->entity_idx == 1)
    { 
      // vec3_copy(obj->collider.box.aabb[0], b1[1]);
      // vec3_copy(obj->collider.box.aabb[1], b1[1]);
      phys_get_final_aabb(obj, b1);
      b1_found = true;
    }
  }
  if (b0_found && b1_found)
  {
    phys_aabb_add(b0, b1, sum_aabb);
    
    phys_obj_t obj = PHYS_OBJ_T_INIT();
    obj.entity_idx = -1;
    vec3_copy(VEC3(0), obj.pos);
    vec3_copy(VEC3(1), obj.scl);
    phys_obj_make_box(sum_aabb, VEC3(0), false, &obj);
    
    phys_debug_draw_box_collider_func(&obj, VEC3(1));
  }

  // -------------------------

  for (u32 i = 0; i < phys_objs_len; ++i)
  {
    phys_obj_t* obj = &phys_objs[i];
    entity_t*   e   = &world[obj->entity_idx];
    
    #ifdef EDITOR
    if (core_data->phys_debug_act)
    {
      if (PHYS_OBJ_HAS_COLLIDER(obj))  { phys_debug_draw_collider(obj); }
		  if (PHYS_OBJ_HAS_RIGIDBODY(obj)) { phys_debug_draw_velocity(obj); }
    }
    #endif

    // either the attached entity or its parent have moved
    if (e->is_moved) // || (world[obj->entity_idx].parent >= 0 && world[world[obj->entity_idx].parent].is_moved) )  
    {
      // @NOTE: this is buggy for some reason
      //        also should i even update the phys obj not the other way around
      // vec3 delta_pos;
      // vec3_sub(obj->pos, e->pos, delta_pos); 
      // vec3_add(delta_pos, obj->pos, obj->pos);         // update physics position after potential transform

      // if (obj->entity_idx == 31) { P_VEC3(e->delta_pos); }
      vec3_add(e->delta_pos, obj->pos, obj->pos);         // update physics position after potential transform
      vec3_add(e->delta_scl, obj->scl, obj->scl);         // update physics scale after potential scaling
      vec3_copy(VEC3(0), e->delta_pos);  
      vec3_copy(VEC3(0), e->delta_scl); 

      e->is_moved = false;

      // @NOTE: needed for ENTITY_SET_POS(), but cant do this every frame, or it stops all movement
      // vec3_copy(VEC3(0), obj->rb.velocity);
    }

    if (PHYS_OBJ_HAS_RIGIDBODY(obj))
    {
      // if (is_moved), etc, was here

      vec3_add(e->delta_force, obj->rb.force, obj->rb.force);   // update physics forces after potential added forces
      vec3_copy(VEC3(0), e->delta_force);  

    }
    // else // if (world[obj->entity_idx].parent >= 0) // static objects with parents
    // {
    //   P_INT(world[obj->entity_idx].parent);
    //   if (world[world[obj->entity_idx].parent].is_moved) { P(" - is moved"); }
    // }
    if (PHYS_OBJ_HAS_COLLIDER(obj))
    {
      e->is_grounded = obj->collider.is_grounded;
    }
    vec3_copy(obj->pos, e->pos);  // update entity position after physics
    ENTITY_SET_POS(e, obj->pos);
  }
}

void program_quit()
{
  glfwSetWindowShouldClose(core_data->window, GLFW_TRUE);
  core_data->program_quit = true;
}

