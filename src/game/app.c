#include "editor/app.h"
// #include "editor/gui.h"
#include "editor/gizmo.h"
#include "editor/terrain_edit.h"
#include "core/program.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/camera.h"
#include "core/file_io.h"
#include "core/assetm.h"
#include "core/state.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/terrain.h"
#include "core/serialization.h"
#include "data/entity_template.h"
#include "phys/phys_world.h"

#include "stb/stb_ds.h"


// bool app_data.wireframe_act = false;
// f32 app_data.mouse_sensitivity = 0.125f;
// int app_data.selected_id = -1; // -1 = not selected

app_data_t app_data = APP_DATA_INIT(); 


static core_data_t* core_data = NULL;

void move_cam_by_keys();
void rotate_cam_by_mouse();

int main(void)
{
#ifdef RELEASE
  program_start(1920, 1800, "mooh", WINDOW_FULL, app_init, app_update);  // WINDOW_FULL
#else
  program_start(1600, 900, "mooh", WINDOW_MIN, app_init, app_update);  // WINDOW_FULL
#endif
  return 0;
}

void app_init()
{
  core_data = core_data_get();
  core_data->phys_act = true;
  core_data->scripts_act = true;
  cubemap_t cube_map = assetm_load_cubemap_hdr("#cubemaps/gothic_manor_01_2k.hdr");
  core_data->cube_map = cube_map;


  // serialization_write_scene_to_file(scene_name);
  serialization_load_scene_from_file(SCENE_FILE_NAME);

  // in game will be done by camera-controller
  // input_center_cursor_pos();
  camera_set_pos(VEC3_XYZ(0.0f,   6.0f,  10.0f));
  camera_set_front(VEC3_XYZ(0.0f,  -0.15f, -1.0f));
  
  // TIMER_FUNC_STATIC(gui_init());

  serialization_load_terrain_from_file(TERRAIN_FILE_NAME);
  TIMER_FUNC_STATIC(terrain_create(25));
}

void app_update()
{
  // TIMER_FUNC(gui_update());

  // -- input --
  input_set_cursor_visible(false);
  rotate_cam_by_mouse(); 
  move_cam_by_keys();
  
  if (input_get_key_pressed(KEY_EXIT))
  {
    program_quit();
  }

}

app_data_t* app_data_get()
{ return &app_data; }

void move_cam_by_keys()
{
  // -- move the cam --
	f32 cam_speed = CAM_SPEED * core_data->delta_t;
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

