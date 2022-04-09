#include "data/test_comp.h"
#include "core/input.h"
#include "math/math_inc.h"


void entity_init(entity_t* this)
{
  P("-- entity init --");
}


void entity_update(entity_t* this, float dt)
{
  const float speed = 10.0f * dt;
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
