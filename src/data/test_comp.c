#include "data/test_comp.h"
#include "core/input.h"
#include "core/camera.h"
#include "math/math_inc.h"




void player_init(entity_t* this)
{
}
void player_update(entity_t* this, f32 dt)
{
  // @NOTE: directly moving object
  // const f32 speed = 10.0f * dt * 10.0f;

  // if (input_get_key_down(KEY_LEFT_ARROW))
  // { ENTITY_MOVE_X(this, -speed); }
	// if (input_get_key_down(KEY_RIGHT_ARROW))
  // { ENTITY_MOVE_X(this, speed); }
	// if (input_get_key_down(KEY_UP_ARROW))
  // { ENTITY_MOVE_Z(this, -speed); }
	// if (input_get_key_down(KEY_DOWN_ARROW))
  // { ENTITY_MOVE_Z(this, speed); }

	// if (input_get_key_down(KEY_SPACE))
  // { 
  //   ENTITY_MOVE_Y(this, speed); 
  // }
  
  // @NOTE: moving object with physics
  const f32 speed = 500.0f * dt;

  if (input_get_key_down(KEY_LEFT_ARROW)  || input_get_key_down(KEY_A))
  { ENTITY_FORCE_X(this, -speed); }
	if (input_get_key_down(KEY_RIGHT_ARROW) || input_get_key_down(KEY_D))
  { ENTITY_FORCE_X(this, speed); }
	if (input_get_key_down(KEY_UP_ARROW)    || input_get_key_down(KEY_W))
  { ENTITY_FORCE_Z(this, -speed); }
	if (input_get_key_down(KEY_DOWN_ARROW)  || input_get_key_down(KEY_S))
  { ENTITY_FORCE_Z(this, speed); }

	if (this->is_grounded && input_get_key_pressed(KEY_SPACE))
  { ENTITY_FORCE_Y(this, speed * 80.0f); }

  vec3 pos;
  vec3_add(this->pos, VEC3_XYZ(0, 2, 8), pos);
  camera_set_pos(pos);
}

void entity_init(entity_t* this)
{
  P("-- entity init --");
}


void entity_update(entity_t* this, f32 dt)
{
  const f32 speed = 10.0f * dt;
  // this->rot[2] += speed;
  ENTITY_ROTATE_Z(this, speed);

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
