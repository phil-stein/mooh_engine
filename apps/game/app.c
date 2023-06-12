#include "game/app.h"
#include "editor/terrain_edit.h"
#include "core/program.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer/renderer.h"
#include "core/camera.h"
#include "core/io/file_io.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/state.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/terrain.h"
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

int main(void)
{
  // program_start(1600, 900, "mooh", WINDOW_MIN, app_init, app_update, ASSET_PATH);  // WINDOW_FULL
  // @BUGG: having 1x1 causes weird white rect on startup i think, not sure though
  program_start(1, 1, "moohch nice game boi", WINDOW_MAX, app_init, app_update, ASSET_PATH);  // WINDOW_FULL
  
  return 0;
}

void app_init()
{
  #ifdef ASSETM_NO_ZIP
  P("| ASSETM NO ZIP");
  #endif
  #ifdef EDITOR
  P("| EDITOR");
  #endif
  #ifdef INCLUDE_PLAY_MODE
  P("| PLAY MODE");
  #endif

  core_data = core_data_get();
  
  const char scene_name[] = "test.scene";
  TIMER_FUNC_STATIC(save_sys_load_scene_from_file(scene_name));
  // TIMER_FUNC_STATIC(save_sys_load_terrain_from_file("test.terrain"));
  // TIMER_FUNC_STATIC(terrain_create(25));

  // in game will be done by camera-controller
  vec3_copy(VEC3_XYZ(0.0f,   6.0f,  10.0f), core_data->cam.pos);
  vec3_copy(VEC3_XYZ(0.0f,  -0.15f, -1.0f), core_data->cam.front);
  
  
  // TIMER_FUNC_STATIC(gui_init());
}

void app_update()
{
  programm_app_default_logic(core_data);

  // -- input --
  // ...
  
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
		vec3 front; vec3_copy(core_data->cam.front, front); // camera_get_front(front);
		vec3_mul_f(front, cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_BACKWARD))
	{
		vec3 front; vec3_copy(core_data->cam.front, front); // camera_get_front(front);
		vec3_mul_f(front, -cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_LEFT))
	{
		// vec3 up;    camera_get_up(up);
		// vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(core_data->cam.front, core_data->cam.up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, -cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_RIGHT))
	{
		// vec3 up;    camera_get_up(up);
		// vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(core_data->cam.front, core_data->cam.up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_DOWN))
	{
		vec3 up;	vec3_copy(core_data->cam.up, up); // camera_get_up(up);
		vec3_mul_f(up, -cam_speed, up);
		camera_move(up);
	}
	if (input_get_key_down(KEY_MOVE_UP))
	{
		vec3 up;	vec3_copy(core_data->cam.up, up); // camera_get_up(up);
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
    vec3_copy(core_data->cam.front, front); // camera_get_front(front);
		pitch = core_data->cam.front[1] * 90; // -30.375f;
		yaw	  = core_data->cam.front[2] * 90; // -90.875;
		init = true;
	}

	vec3 dir;
	f32 yaw_rad = yaw;     m_deg_to_rad(&yaw_rad);
	f32 pitch_rad = pitch; m_deg_to_rad(&pitch_rad);

	dir[0] = (f32)cos(yaw_rad) * (f32)cos(pitch_rad);
	dir[1] = (f32)sin(pitch_rad);
	dir[2] = (f32)sin(yaw_rad) * (f32)cos(pitch_rad);
	// camera_set_front(dir);
  vec3_copy(dir, core_data->cam.front);

}

