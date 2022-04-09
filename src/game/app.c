#include "game/app.h"
#include "game/gui.h"
#include "core/program.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/time.h"
#include "core/camera.h"
#include "core/file_io.h"
#include "core/assetm.h"
#include "core/state.h"

#include "stb/stb_ds.h"


bool wiref_act = false;

float mouse_sensitivity = 0.125f;

int blank_tex;
int blank_black_tex;
int blank_grey_tex;
int quad_mesh;
int sphere_mesh;

int shotgun_id = 0;


int main(void)
{
  program_start(1920, 1080, "mooh", WINDOW_MAX, app_init, app_update);  // WINDOW_FULL

  return 0;
}
void app_init()
{
  // cubemap_t cube_map = assetm_load_cubemap_hdr("Newport_Loft_Ref.hdr");
  cubemap_t cube_map = assetm_load_cubemap_hdr("gothic_manor_01_2k.hdr");
  state_set_cubemap(cube_map);

  // -- add entities --
  state_add_entity_from_template(VEC3_XYZ(0, 0, 0), VEC3_XYZ(-90, 0, 0), VEC3(10), 0);
  
  state_add_entity_from_template(VEC3_XYZ(0, 4, 0), VEC3_XYZ(0, 0, 0), VEC3(1), 1);

  state_add_entity_from_template(VEC3_XYZ(-2, 4, 0), VEC3_XYZ(0, 0, 0), VEC3(1), 2);
  
  state_add_entity_from_template(VEC3_XYZ(2, 0, 0), VEC3_XYZ(0, 0, 0), VEC3(0.5f), 3);
  
  state_add_entity_from_template(VEC3_XYZ(5.5f, 4, 0), VEC3_XYZ(-90, 0, 90), VEC3(0.35f), 4);
  
  state_add_entity_from_template(VEC3_XYZ(-4, -0.2f, 0), VEC3_XYZ(0, 0, 0), VEC3(3), 5);
  
  // state_entity_add_child(gun_id, char_id);
  // state_entity_add_child(char_id, sphere_id00);
  // state_entity_remove_child(gun_id, char_id);
  
  // -- add lights --
  state_add_dir_light(VEC3_XYZ(0, 10, 0), VEC3_XYZ(0.2f, 1.0f, 0.2f), RGB_F(1.0f, 0.6f, 0.6f), 8.5f, true, 2048, 2048);
  // state_add_dir_light(VEC3_XYZ(0, 10, 0), VEC3_XYZ(-0.2f, 1.0f, -0.2f), RGB_F_RGB(1.0f), 0.5f, false, 0, 0);
  
  state_add_point_light(VEC3_XYZ(-10,  10, 10), RGB_F_RGB(300), 1.0f);
  // state_add_point_light(VEC3_XYZ( 10,  10, 10), RGB_F_RGB(300), 1.0f);
  // state_add_point_light(VEC3_XYZ(-10, -10, 10), RGB_F_RGB(300), 1.0f);
  // state_add_point_light(VEC3_XYZ( 10, -10, 10), RGB_F_RGB(300), 1.0f);
  
  // hide cursor
  input_center_cursor_pos();
  input_set_cursor_visible(false);
  camera_set_pos(VEC3_XYZ(0.0f,   6.0f,  10.0f));
  camera_set_front(VEC3_XYZ(0.0f,  -0.15f, -1.0f));

  #ifdef EDITOR
  gui_init();
  #endif
}

void app_update()
{
  #ifdef EDITOR
  gui_update();
  #endif 


  // -- input --

  // tmp
  if (input_get_key_pressed(KEY_SPACE))
  {
    cubemap_t cube_map;
    static bool idx = true;
    if (idx)
    { cube_map = assetm_load_cubemap_hdr("Newport_Loft_Ref.hdr"); }
    else
    { cube_map = assetm_load_cubemap_hdr("gothic_manor_01_2k.hdr"); }
    state_set_cubemap(cube_map);
    idx = !idx;
  }


  rotate_cam_by_mouse();

  if (input_get_key_pressed(KEY_TAB))
	{
		wiref_act = !wiref_act;
		renderer_set_wireframe(wiref_act);
	}
  if (input_get_key_pressed(KEY_ESCAPE))
  {
    program_quit();
  }

  // -- move the cam --
	float cam_speed = 2.5f * time_get_delta_t();
	if (input_get_key_down(KEY_LEFT_SHIFT))
	{ cam_speed *= 3; }
	if (input_get_key_down(KEY_W))
	{
		vec3 front; camera_get_front(front);
		vec3_mul_f(front, cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_S))
	{
		vec3 front; camera_get_front(front);
		vec3_mul_f(front, -cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_A))
	{
		vec3 up;    camera_get_up(up);
		vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(front, up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, -cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_D))
	{
		vec3 up;    camera_get_up(up);
		vec3 front; camera_get_front(front);
		vec3 dist;
		vec3_cross(front, up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_Q))
	{
		vec3 up;	camera_get_up(up);
		vec3_mul_f(up, -cam_speed, up);
		camera_move(up);
	}
	if (input_get_key_down(KEY_E))
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

	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;

	
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

