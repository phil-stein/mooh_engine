#include "phys/phys_dynamics.h"
#include "phys/phys_debug_draw.h"

// #include "global/global.h"
#include "phys/phys_types.h"

const vec3 gravity = { 0.0f, -9.89f, 0.0f };

// ---- dynamics ----

void phys_dynamics_simulate(phys_obj_t* obj, f32 dt)
{
	if (!HAS_FLAG(obj->flags, PHYS_HAS_RIGIDBODY)) { return; }
  // ERR_CHECK(obj->rb.mass > 1.0f, "mass: %f, id: %d\n", obj->rb.mass, obj->entity_idx);
  // PF("id: %d, mass: %f\n", obj->entity_idx, obj->rb.mass);
  // PF("id: %d, ", obj->entity_idx); P_VEC3(obj->rb.velocity);
  // P_COLLIDER_TYPE_T(obj->collider.type);
  // P_COLLIDER_T(obj->collider);
  // P_RIGIDBODY_T(obj->rb);
  // P_PHYS_OBJ_FLAGS_T(obj->flags);
  // P_PHYS_OBJ_T(obj);
	// add mass * gravity to force
	vec3 mass = { obj->rb.mass, obj->rb.mass , obj->rb.mass };
	vec3 mg;
	vec3_mul(mass, (f32*)gravity, mg);
	vec3_add(obj->rb.force, mg, obj->rb.force);

	// add (force / mass) * delta_t to velocity
	vec3 fdm;
	vec3_div(obj->rb.force, mass, fdm);
	vec3 delta_t = { dt, dt, dt };
	vec3_mul(fdm, delta_t, fdm);
	vec3_add(obj->rb.velocity, fdm, obj->rb.velocity);

	// position += velocity * delta_t
	vec3 v;
	vec3_mul(obj->rb.velocity, delta_t, v);
	vec3_add(obj->pos, v, obj->pos);

	// reset force to (0, 0, 0)
	vec3_copy(VEC3(0), obj->rb.force);

  phys_debug_draw_velocity(obj);
}
