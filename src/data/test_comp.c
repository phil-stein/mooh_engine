#include "data/test_comp.h"
#include "core/input.h"
#include "core/camera.h"
#include "core/serialization.h"
#include "core/debug/debug_draw.h"
#include "math/math_inc.h"


vec3 start_pos = { 0, 0, 0 }; // starting position of player char

void player_camera(entity_t* this, f32 dt);

void player_init(entity_t* this)
{
  vec3_copy(this->pos, start_pos);
  input_center_cursor_pos(); 
  input_set_cursor_visible(false);
}
void player_update(entity_t* this, f32 dt)
{
  // @NOTE: moving object with physics
  f32 speed      = 500.0f * dt;
  f32 jump_force = 600.0f * 80.0f * dt;
  if (input_get_key_down(KEY_LEFT_SHIFT))
  { speed *= 4.0f; }

  vec3 front;
  vec3 back;
  vec3 left;
  vec3 right;
  mat4_get_directions(this->model, front, back, left, right);
  // vec3 test;
  // vec3_mul_f(front, 2.0f, test);
  // vec3_add(this->pos, test, test);
  // debug_draw_line_register(this->pos, test, RGB_F(1, 0, 1));
  
  vec3_mul_f(front, speed, front);
  vec3_mul_f(back, speed, back);
  vec3_mul_f(left, speed, left);
  vec3_mul_f(right, speed, right);

  if (input_get_key_down(KEY_LEFT_ARROW)  || input_get_key_down(KEY_A))
  { ENTITY_FORCE(this, left); }
	if (input_get_key_down(KEY_RIGHT_ARROW) || input_get_key_down(KEY_D))
  { ENTITY_FORCE(this, right); }
	if (input_get_key_down(KEY_UP_ARROW)    || input_get_key_down(KEY_W))
  { ENTITY_FORCE(this, front); }
	if (input_get_key_down(KEY_DOWN_ARROW)  || input_get_key_down(KEY_S))
  { ENTITY_FORCE(this, back); }
	
  if (this->is_grounded && input_get_key_pressed(KEY_SPACE))
  { ENTITY_FORCE_Y(this, jump_force); }
  
  // @NOTE: set camera orientation
  vec3 cam_pos; 
  camera_get_pos(cam_pos);
  vec3 orientation = { 0.0f, -0.3f, -1.0f };
  vec3 target;
  vec3_add(this->pos, VEC3_Y(3), target); // so not looking at feet
  vec3_sub(target, cam_pos, orientation);
  vec3_normalize(orientation, orientation);
  camera_set_front(orientation);

  // -- mouse control --
  f32 x_offset = input_get_mouse_delta_x();
  f32 y_offset = input_get_mouse_delta_y();
  
  input_center_cursor_pos(); 
  input_set_cursor_visible(false);
  
  f32 mouse_sensitivity = 0.5f;
  x_offset *= mouse_sensitivity;
  y_offset *= mouse_sensitivity;

  ENTITY_ROTATE_Y(this, -x_offset * 0.75f);

  // @NOTE: rotate cam around player x-axis
  // - get player model
  // - rotate by x degree on x-axis
  // - get forward vector
  // - use vec to position cam
  
  static f32 camera_rot = 0.0f;
  camera_rot = CLAMP(camera_rot + y_offset, -80, 10);
  vec3 pos, rot;
  vec3_copy(this->pos, pos);
  vec3_copy(this->rot, rot);
  rot[0] -= camera_rot;

  
  mat4 player_model;
  mat4_make_model(pos, rot, VEC3(1), player_model);
  
  vec3_copy(VEC3(0), cam_pos);
  cam_pos[2] -= 10;  // offset to the back
  mat4 cam_model;
  mat4_make_model(cam_pos, VEC3(0), VEC3(1), cam_model);
  mat4_mul(player_model, cam_model, cam_model);

  // re-assign front & back as the player_model is different from the this->model
  mat4_get_forward(player_model, front);
  vec3_mul_f(front, -1.0f, back);

  vec3_mul_f(back, 10, pos);
  vec3_add(pos, this->pos, pos);
  
  // debug_draw_sphere_register(pos, 0.35f, RGB_F(0, 1, 1));
  camera_get_pos(cam_pos);
  vec3 dif;
  vec3_sub(pos, cam_pos, dif);
  // @NOTE: speed of cam-smoothing framerate dependent, but gets yittery when * dt
  vec3_mul_f(dif, 0.1f, dif); // CLAMP(8.0f * dt, 0.08f, 0.2f)
  // P_F32(8.0f * dt);
  // P_F32(CLAMP(8.0f * dt, 0.08f, 0.2f));
  vec3_add(cam_pos, dif, cam_pos);
  camera_set_pos(cam_pos);

  // @NOTE: reset when falling down
  if (this->pos[1] < -2.0f)
  { 
    serialization_load_scene_from_file("test.scene");
    // ENTITY_SET_POS(this, start_pos); 
  }

}

void player_on_collision(entity_t* this, entity_t* col)
{
  // PF("player_on_collision: %d\n", col->id);
}
void player_on_trigger(entity_t* this, entity_t* trig)
{
  // PF("player_on_trigger: %d\n", trig->id);

  ENTITY_FORCE_Y(this, 50.0f);
}


void player_camera(entity_t* this, f32 dt)
{
  // // @NOTE: rotates the camera accoding to the mouse-movement
	// static bool init = false;
	// static f32 pitch, yaw;

	// f32 x_offset = input_get_mouse_delta_x();
	// f32 y_offset = input_get_mouse_delta_y();

  // f32 mouse_sensitivity = 0.5f;
	// x_offset *= mouse_sensitivity;
	// y_offset *= mouse_sensitivity;

	// yaw += x_offset;
	// pitch += y_offset;

	// // printf("pitch: %f, yaw: %f\n", pitch, yaw);

	// if (pitch > 89.0f)
	// { pitch = 89.0f; }
	// if (pitch < -89.0f)
	// { pitch = -89.0f; }

	// if (!init)
	// {
  //   vec3 front;
  //   camera_get_front(front);
	// 	pitch = front[1] * 90; // -30.375f;
	// 	yaw	  =	front[2] * 90; // -90.875;
	// 	init = true;
	// }

	// vec3 dir;
	// f32 yaw_rad = yaw;     m_deg_to_rad(&yaw_rad);
	// f32 pitch_rad = pitch; m_deg_to_rad(&pitch_rad);

	// dir[0] = (f32)cos(yaw_rad) * (f32)cos(pitch_rad);
  // dir[1] = (f32)sin(pitch_rad);
	// dir[2] = (f32)sin(yaw_rad) * (f32)cos(pitch_rad);
	// camera_set_front(dir);
}

void entity_init(entity_t* this)
{
  P("-- entity init --");
}


void entity_update(entity_t* this, f32 dt)
{
  const f32 speed = 10.0f * dt;
  // this->rot[2] += speed;
  ENTITY_ROTATE_Y(this, speed);

  // f32 r = (f32)sin(time_get_total_t());
  // f32 g = (f32)cos(time_get_total_t());
  // f32 b = (f32)sin(time_get_total_t());
  // r = (r + 1) * 0.5f;
  // g = (g + 1) * 0.5f;
  // b = (b + 1) * 0.5f;
  // 
  // vec3_copy(RGB_F(r, g, b), this->tint);

  // this->pos[1] = r * 0.8f +4;

	// if (input_get_key_down(KEY_LEFT_ARROW))
  // { this->pos[0] -= speed; }
	// if (input_get_key_down(KEY_RIGHT_ARROW))
  // { this->pos[0] += speed; }
	// if (input_get_key_down(KEY_UP_ARROW))
  // { this->pos[2] -= speed; }
	// if (input_get_key_down(KEY_DOWN_ARROW))
  // { this->pos[2] += speed; }

}
