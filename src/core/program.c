#include "core/program.h"
#include "core/core_data.h"
#include "core/renderer.h"
#include "core/input.h"
#include "core/assetm.h"
#include "core/state.h"
#include "core/terrain.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "math/math_inc.h"
#include "phys/phys_world.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"

#include <time.h>


static core_data_t* core_data = NULL;

void program_start(int width, int height, const char* title, window_type type, empty_callback* init_f, empty_callback* update_f)
{
	if (!window_create(width, height, title, type))
	{
		printf("[ERROR] window creation failed\n");
		return;
	}

	// ---- init ----
  rand_seed(time(NULL));
	TIMER_FUNC_STATIC(input_init());
  TIMER_FUNC_STATIC(assetm_init());
  
  TIMER_FUNC_STATIC(core_data_init());
  core_data = core_data_get();
	
  TIMER_FUNC_STATIC(renderer_init());
  TIMER_FUNC_STATIC(terrain_init());
	TIMER_FUNC_STATIC(init_f());     // init callback
  TIMER_FUNC_STATIC(state_init());
  TIMER_FUNC_STATIC(debug_draw_init());

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
  int world_len = 0;
  entity_t* world= state_get_entity_arr(&world_len);
  u32 phys_objs_len = 0;
  phys_obj_t* phys_objs = phys_get_obj_arr(&phys_objs_len);
  for (u32 i = 0; i < phys_objs_len; ++i)
  {
    phys_obj_t* obj = &phys_objs[i];
    // if (world[obj->entity_idx].is_moved)
    // {
      // vec3_copy(world[obj->entity_idx].pos, obj->pos);  // update physics position after potential transform
      // vec3_copy(world[obj->entity_idx].scl, obj->scl);  // update physics scale after potential scaling
      vec3_add(world[obj->entity_idx].delta_pos, obj->pos, obj->pos);  // update physics position after potential transform
      vec3_add(world[obj->entity_idx].delta_scl, obj->scl, obj->scl);  // update physics scale after potential scaling
      vec3_copy(VEC3(0), world[obj->entity_idx].delta_pos);  
      vec3_copy(VEC3(0), world[obj->entity_idx].delta_scl);  
    // }
    vec3_copy(obj->pos, world[obj->entity_idx].pos);  // update entity position after physics
    ENTITY_SET_POS(&world[obj->entity_idx], obj->pos);
  }
}

void program_quit()
{
  glfwSetWindowShouldClose(core_data->window, GLFW_TRUE);
  core_data->program_quit = true;
}

