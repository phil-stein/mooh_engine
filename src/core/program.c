#include "core/program.h"
#include "core/core_data.h"
#include "core/camera.h"
#include "core/renderer.h"
#include "core/renderer_direct.h"
#include "core/renderer_extra.h"
#include "core/serialization.h"
#include "core/input.h"
#include "core/assetm.h"
#include "core/state.h"
#include "core/terrain.h"
#include "core/event_sys.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "math/math_inc.h"
#include "phys/phys_world.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"

#include <time.h>

#define STB_DS_IMPLEMENTATION       // only define once
#include "stb/stb_ds.h"         


static core_data_t* core_data = NULL;

void program_start(int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f, const char* asset_path)
{
  TIMER_START(" -- program init -- ");

  if (!window_create(width, height, title, type))
	{
		printf("[ERROR] window creation failed\n");
		return;
	}
 
	// ---- init ----

  // asset path
  core_data = core_data_get();
  strcpy(core_data->asset_path, asset_path);
  sprintf(core_data->shaders_path, "%sshaders/", asset_path);
 

  rand_seed(time(NULL));
	TIMER_FUNC_STATIC(input_init());
  TIMER_FUNC_STATIC(assetm_init());
  
  TIMER_FUNC_STATIC(core_data_init());
	TIMER_FUNC_STATIC(serialization_init());
  TIMER_FUNC_STATIC(renderer_direct_init());
  TIMER_FUNC_STATIC(renderer_extra_init());
  TIMER_FUNC_STATIC(camera_init());
  TIMER_FUNC_STATIC(renderer_init());
  TIMER_FUNC_STATIC(terrain_init());
	TIMER_FUNC_STATIC(init_f());     // init callback
  TIMER_FUNC_STATIC(state_init());
  TIMER_FUNC_STATIC(debug_draw_init());
  
  TIMER_FUNC_STATIC(phys_init(event_sys_trigger_phys_collision, event_sys_trigger_phys_trigger)); 

  TIMER_STOP_STATIC();  // program init timer

  char _title[64];
  sprintf(_title, "game :)");

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
  
    // --- title ----
    sprintf(_title, "%s | fps: '%.1f'", title, core_data->cur_fps);
    window_set_title(_title);

		// ---- update ----
    TIMER_FUNC(terrain_update());

    TIMER_FUNC(renderer_update());
    TIMER_FUNC(debug_draw_update());
    
    TIMER_FUNC(update_f());   // update callback
    TIMER_FUNC(state_update(core_data->delta_t));
#ifdef EDITOR
    if (core_data->phys_act)
    {
#endif
      TIMER_FUNC(phys_update(core_data->delta_t));
      TIMER_FUNC(program_sync_phys());
#ifdef EDITOR
    }
#endif
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
  entity_t* world= state_get_entity_arr(&world_len, &world_dead_len);
  u32 phys_objs_len = 0;
  phys_obj_t* phys_objs = phys_get_obj_arr(&phys_objs_len);
  for (u32 i = 0; i < phys_objs_len; ++i)
  {
    phys_obj_t* obj = &phys_objs[i];
    entity_t*   e   = &world[obj->entity_idx];

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

