#include "tex_viewer/app.h"

#include "core/program.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_direct.h"
#include "core/camera.h"
#include "core/io/file_io.h"
#include "core/io/assetm.h"
#include "core/io/asset_io.h"
#include "core/io/save_sys.h"
#include "core/state.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/terrain.h"
#include "core/debug/debug_opengl.h"
#include "data/entity_template.h"
#include "phys/phys_world.h"

#include "stb/stb_ds.h"

#include "GLAD/glad.h"

#include <direct.h>

// bool app_data.wireframe_act = false;
// float app_data.mouse_sensitivity = 0.125f;
// int app_data.selected_id = -1; // -1 = not selected

app_data_t app_data = APP_DATA_INIT(); 


static core_data_t* core_data = NULL;

char tex_path[512] = "\\Workspace\\C\\mooh_engine\\assets\\textures\\#internal\\preview_bg.tex";

texture_t tex;

vec3 cam_pos  = { 0, 0, 0 };
vec2 tex_pos  = { 0, 0 };
vec3 tex_size = { 4, 4 };

void move_cam_by_keys();
void rotate_cam_by_mouse();

int main(int argc, char** argv)
{
  P_INT(argc);
  P_STR(argv[1]);
  u32 arg_len = strlen(argv[1]);
  bool has_ending = ( argv[1][arg_len -4] == '.' &&
                      argv[1][arg_len -3] == 't' &&
                      argv[1][arg_len -2] == 'e' &&
                      argv[1][arg_len -1] == 'x' );
  sprintf(tex_path, "%s\\%s%s", _getcwd(NULL, 0), argv[1], has_ending ? "" : ".tex");
  // ASSERT(argc >= 2);
  // strcpy(tex_path, argv[1]);
  P_STR(tex_path);

  P_STR(ASSET_PATH);
  program_start(1600, 900, "mooh", WINDOW_MIN, app_init, app_update, ASSET_PATH);  // WINDOW_FULL
  return 0;
}

void app_init()
{
  core_data = core_data_get();
  
  // tex = asset_io_load_texture("#internal/brdf_lut.tex", false);
  // tex = asset_io_load_texture("stylized_brick/stylized_bricks_albedo.jpg", false);
  
  tex = asset_io_load_texture_full_path(tex_path, false);

}

void app_update()
{
  int w, h;
  window_get_size(&w, &h);
  _glViewport(0, 0, w, h);
  _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  renderer_direct_draw_quad_textured(cam_pos, tex_pos, tex_size, &tex);

  // -- input --
	
  float cam_speed = CAM_SPEED * core_data->delta_t;
  if (input_get_key_down(KEY_UP_ARROW))
	{
    cam_pos[1] -= cam_speed;
	}
	if (input_get_key_down(KEY_DOWN_ARROW))
	{
    cam_pos[1] += cam_speed;
	}
	if (input_get_key_down(KEY_LEFT_ARROW))
	{
    cam_pos[0] -= cam_speed;
	}
	if (input_get_key_down(KEY_RIGHT_ARROW))
	{
    cam_pos[0] += cam_speed;
	}

  if (input_get_key_pressed(KEY_EXIT))
  {
    program_quit();
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
	camera_set_front(dir);
}

