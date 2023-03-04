#include "editor/app.h"
#include "editor/gui.h"
#include "editor/gizmo.h"
#include "editor/terrain_edit.h"
#include "core/program.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/renderer_extra.h"
#include "core/camera.h"
#include "core/file_io.h"
#include "core/assetm.h"
#include "core/state.h"
#include "core/event_sys.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/terrain.h"
#include "core/serialization.h"
#include "data/entity_template.h"
#include "phys/phys_world.h"

#include "stb/stb_ds.h"


// bool app_data.wireframe_act = false;
// float app_data.mouse_sensitivity = 0.125f;
// int app_data.selected_id = -1; // -1 = not selected

app_data_t app_data = APP_DATA_INIT(); 


static core_data_t* core_data = NULL;

void move_cam_by_keys();
void rotate_cam_by_mouse();

// @TMP: testing event_sys
void added(int id)           { PF("! added entity: %d\n", id); }
void removed(int id)         { PF("! removed entity: %d\n", id); }
void parented(int p, int c)  { PF("! parented entity: %d, %d\n", p, c); }
void parent_rm(int p, int c) { PF("! parent removed entity: %d, %d\n", p, c); }

int main(void)
{
  program_start(1600, 900, "mooh", WINDOW_MIN, app_init, app_update, ASSET_PATH);  // WINDOW_FULL
  
  return 0;
}

void app_init()
{
  // @TMP: 
  event_sys_register_entity_added(added);
  event_sys_register_entity_removed(removed);
  event_sys_register_entity_parented(parented);
  event_sys_register_entity_parent_removed(parent_rm);
  
  core_data = core_data_get();
  // pause physics and scripts on start
  // core_data->phys_act = false;
  // core_data->scripts_act = false;
  // @TODO: do this in program_start()
  //        maybe not, it'll be done load_scene()
  TIMER_FUNC_STATIC(cubemap_t cube_map = assetm_load_cubemap_hdr("#cubemaps/gothic_manor_01_2k.hdr"));
  // state_set_cubemap(cube_map);
  core_data->cube_map = cube_map;

  // // -- add entities --
  // // int quad_id     = 
  // // TIMER_FUNC_STATIC(state_add_entity_from_template(VEC3_XYZ(0, 0, 0), VEC3_XYZ(-90, 0, 0), VEC3(10), 0));

  // int sphere01_id = state_add_entity_from_template(VEC3_XYZ(0, 4, 0), VEC3_XYZ(0, 0, 0), VEC3(1), 1);

  // int sphere02_id = state_add_entity_from_template(VEC3_XYZ(-2, 4, 0), VEC3_XYZ(0, 0, 0), VEC3(1), 2);

  // // int demon01_id  = 
  // TIMER_FUNC_STATIC(state_add_entity_from_template(VEC3_XYZ(2, 0, 0), VEC3_XYZ(-90, 0, 0), VEC3(1), 3));

  // // int shotgun_id  = 
  // state_add_entity_from_template(VEC3_XYZ(5.5f, 4, 0), VEC3_XYZ(-90, 0, 90), VEC3(1), 4);

  // int demon02_id = state_add_entity_from_template(VEC3_XYZ(-4, -0.2f, 0), VEC3_XYZ(-90, 0, 0), VEC3(1), 5);
  // 
  // state_add_entity_from_template(VEC3_XYZ(0, 0, -4), VEC3_XYZ(-90, 0, 0), VEC3(2), ENTITY_TEMPLATE_STONE01);
  // 
  // int stone02_id = state_add_entity_from_template(VEC3_XYZ(0, 10, -4), VEC3_XYZ(-90, 0, 0), VEC3(2), ENTITY_TEMPLATE_STONE01);
  // phys_add_obj_rb(stone02_id, VEC3_XYZ(0, 10, -4), 1.0f);
  // 
  // state_add_entity_from_template(VEC3_XYZ(3, 0.5f, -4), VEC3_XYZ(-90, 0, 0), VEC3(1), ENTITY_TEMPLATE_TREE01);
 
  // // state_entity_add_child(shotgun_id, demon01_id);
  // 
  // state_entity_add_child(demon02_id, sphere02_id);    
  // state_entity_add_child(sphere02_id, sphere01_id);
  // // state_entity_add_child(sphere01_id, quad_id);
  // 
  // // state_entity_remove_child(sphere02_id, sphere01_id);
  // // state_add_entity_from_template(VEC3_XYZ(2, 0, -2), VEC3_XYZ(-90, 0, 0), VEC3(1), 3);

  // // -- add lights --
  // state_add_dir_light(VEC3_XYZ(0, 10, 0), VEC3_XYZ(0.2f, 1.0f, 0.2f), RGB_F(1.0f, 0.6f, 0.6f), 8.5f, true, 2048, 2048);
  // // state_add_dir_light(VEC3_XYZ(0, 10, 0), VEC3_XYZ(-0.2f, 1.0f, -0.2f), RGB_F_RGB(1.0f), 0.5f, false, 0, 0);
  // 
  // state_add_point_light(VEC3_XYZ(-10,  10, 10), RGB_F_RGB(300), 1.0f);
  // // state_add_point_light(VEC3_XYZ( 10,  10, 10), RGB_F_RGB(300), 1.0f);
  // // state_add_point_light(VEC3_XYZ(-10, -10, 10), RGB_F_RGB(300), 1.0f);
  // // state_add_point_light(VEC3_XYZ( 10, -10, 10), RGB_F_RGB(300), 1.0f);

  // 
  // state_add_dir_light(VEC3_XYZ(0, 10, 0), VEC3_XYZ(0.2f, 1.0f, 0.2f), RGB_F(1.0f, 0.6f, 0.6f), 8.5f, true, 2048, 2048);
  // state_add_point_light(VEC3_XYZ(-10,  10, 10), RGB_F_RGB(300), 1.0f);
 
  const char scene_name[] = "test.scene";
  // serialization_write_scene_to_file(scene_name);
  TIMER_FUNC_STATIC(serialization_load_scene_from_file(scene_name));

  // in game will be done by camera-controller
  // input_center_cursor_pos();
  camera_set_pos(VEC3_XYZ(0.0f,   6.0f,  10.0f));
  camera_set_front(VEC3_XYZ(0.0f,  -0.15f, -1.0f));
  
  TIMER_FUNC_STATIC(gui_init());

  // -- terrain --

  // vec2* info = malloc(sizeof(vec2) * TERRAIN_LAYOUT_VERT_INFO_LEN(core_data));
  // for (int i = 0; i < TERRAIN_LAYOUT_VERT_INFO_LEN(core_data); ++i)
  // { info[i][0] = 0.0f; info[i][1] = 0.0f; }
  // // { height[i] = (u8)(rand_f32() * 256.0f); }

  // const int size = 10;
  // for (int x = -(size/2); x < (size/2); ++x)
  // {
  //   for (int z = -(size/2); z < (size/2); ++z)
  //   {
  //     terrain_layout_t layout = { .pos = { x, z }, .vert_info = info  };
  //     arrput(core_data->terrain_layout, layout);
  //   }
  // }
  // terrain_layout_t layout = { .pos = { 0, 0 }, .vert_info = info  };
  // arrput(core_data->terrain_layout, layout);

  // // core_data->terrain_layout_len = arrlen(core_data->terrain_layout);
  // // core_data->terrain_layout_len = 0;
  // terrain_add_chunk(IVEC2(0));
  // P_INT(core_data->terrain_layout_len);
  // TIMER_FUNC_STATIC(terrain_create(25));
  // core_data->terrain_scl = 100;
  
  TIMER_FUNC_STATIC(serialization_load_terrain_from_file("test.terrain"));
  TIMER_FUNC_STATIC(terrain_create(25));

}



void app_update()
{

  // @TODO: this shows infront of gizmos
  // draw lights
  if (!core_data_is_play())
  {
    int world_len = 0;
    int world_dead_len = 0;
    entity_t* world = state_get_entity_arr(&world_len, &world_dead_len);
    for (int i = 0; i < world_len; ++i)
    {
      if (world[i].point_light_idx >= 0) 
      {
        bool error = false;
        point_light_t* p = state_get_point_light(world[i].point_light_idx, &error); ASSERT(!error);
        debug_draw_mesh_register(world[i].pos, VEC3_XYZ(90, 0, 0), VEC3(2.35f), p->color, assetm_get_mesh_idx("gizmos/lightbulb.fbx")); 
      }
    }
  }

  mat4 model;
  vec3 pos;
  GIZMO_MODEL_POS(&app_data, model, pos);
  TIMER_FUNC(renderer_extra_draw_scene_mouse_pick(model)); 
  TIMER_FUNC(gui_update());
  TIMER_FUNC(gizmo_update());
  TIMER_FUNC(terrain_edit_update());

  // @NOTE: sync selected with outline
  core_data->outline_id = app_data.selected_id;


  // -- input --
  
  // // @TMP: print all entites and their children
  // if (input_get_key_pressed(KEY_SPACE))
  // {
  //   P("|entities start|");
  //   int dead_len, world_len = 0;
  //   entity_t* world = state_get_entity_arr(&world_len, &dead_len);
  //   entity_t* e = NULL;
  //   for (int i_0 = 0; i_0 < world_len; ++i_0)
  //   {
  //     e = &world[i_0];
  //     if (e->is_dead) { continue; }
  //     PF(" -> id: %d, children_len: %d", e->id, e->children_len);
  //     for (int i_1 = 0; i_1 < e->children_len; ++i_1)
  //     {
  //       PF(" | [%d]: %d", i_1, e->children[i_1]);
  //     }
  //     P("");
  //   }
  //   P("|entities end|");
  // }

  if (input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_S) && !core_data_is_play())
  { 
    serialization_write_scene_to_file(SCENE_FILE_NAME); 
    serialization_write_terrain_to_file(TERRAIN_FILE_NAME); 
  }

  if (input_get_key_pressed(KEY_SPACE))
  { core_data->show_shadows = !core_data->show_shadows; }

  // @TODO: flickers first frame
  static bool start = true;
  if (!app_data.mouse_over_ui && input_get_mouse_down(KEY_MOUSE_MOVE_START))
  {
    app_data.switch_gizmos_act = false;
    if (start)
    { 
      input_center_cursor_pos(); 
      start = false;
    }
    else
    {
      input_set_cursor_visible(false);
      rotate_cam_by_mouse(); 
      move_cam_by_keys();
    }
  }
  else
  { 
    app_data.switch_gizmos_act = true;
    start = true;
    input_set_cursor_visible(true); 
  }

  // snapping enabled when holding ctrl
  app_data.gizmo_snapping = (app_data.selected_id >= 0 && input_get_key_down(KEY_GIZMO_SNAPPING));
  
    // duplicate with 'ctrl + d'
  if (app_data.selected_id >= 0 && input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_D))
  {
    int id = state_duplicate_entity(app_data.selected_id, VEC3_XYZ(2, 0, 0));
    app_data.selected_id = id;
  }

  if (input_get_key_pressed(KEY_WIREFRAME_TOGGLE))
	{
		app_data.wireframe_act = !app_data.wireframe_act;
		core_data->wireframe_mode_enabled = app_data.wireframe_act;
	}

  if (input_get_key_pressed(KEY_TOGGLE_FULLSCREEN))
  {
    window_type type = window_get_mode();
    
    P_WINDOW_TYPE(type); 
    
    // @NOTE: min -> max -> full
    type = type == WINDOW_MIN ? WINDOW_MAX : type == WINDOW_MAX ? WINDOW_FULL : WINDOW_MAX;
    
    P_WINDOW_TYPE(type); 
    P("------------");

    window_set_mode(type);
  }

  if (input_get_key_pressed(KEY_EXIT))
  {
    if (core_data->scripts_act || core_data->phys_act)
    { core_data_pause(); } // { core_data->scripts_act = false;  core_data->phys_act = false; }
    else { program_quit(); }
  }

}

app_data_t* app_data_get()
{ return &app_data; }


void move_cam_by_keys()
{
  // -- move the cam --
	float cam_speed = CAM_SPEED * core_data->delta_t;
	if (input_get_key_down(KEY_LEFT_SHIFT))
	{ cam_speed *= CAM_SPEED_SHIFT_MULT; }
	if (input_get_key_down(KEY_MOVE_FORWARD))
	{
		vec3 front; camera_get_front(front);
		vec3_mul_f(front, cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_BACKWARD))
	{
		vec3 front; camera_get_front(front);
		vec3_mul_f(front, -cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_LEFT))
	{
		vec3 up;    camera_get_up(up);
		vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(front, up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, -cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_RIGHT))
	{
		vec3 up;    camera_get_up(up);
		vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(front, up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_DOWN))
	{
		vec3 up;	camera_get_up(up);
		vec3_mul_f(up, -cam_speed, up);
		camera_move(up);
	}
	if (input_get_key_down(KEY_MOVE_UP))
	{
		vec3 up; camera_get_up(up);
		vec3_mul_f(up, cam_speed, up);
		camera_move(up);
	}
}

// rotates the camera accoding to the mouse-movement
void rotate_cam_by_mouse()
{
	static bool init = false;
	static f32 pitch, yaw;

	f32 xoffset = input_get_mouse_delta_x();
	f32 yoffset = input_get_mouse_delta_y();

	xoffset *= app_data.mouse_sensitivity;
	yoffset *= app_data.mouse_sensitivity;

	
	yaw += xoffset;
	pitch += yoffset;

	// printf("pitch: %f, yaw: %f\n", pitch, yaw);

	if (pitch > 89.0f)
	{ pitch = 89.0f; }
	if (pitch < -89.0f)
	{ pitch = -89.0f; }

	if (!init)
	{
    vec3 front;
    camera_get_front(front);
		pitch = front[1] * 90; // -30.375f;
		yaw	  =	front[2] * 90; // -90.875;
		init = true;
	}

	vec3 dir;
	f32 yaw_rad = yaw;     m_deg_to_rad(&yaw_rad);
	f32 pitch_rad = pitch; m_deg_to_rad(&pitch_rad);

	dir[0] = (f32)cos(yaw_rad) * (f32)cos(pitch_rad);
	dir[1] = (f32)sin(pitch_rad);
	dir[2] = (f32)sin(yaw_rad) * (f32)cos(pitch_rad);
	camera_set_front(dir);
}

