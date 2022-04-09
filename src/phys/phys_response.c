#include "phys_response.h"


// ---- collision response ----

void phys_collision_response(phys_obj_t* obj0, phys_obj_t* obj1, collision_info_t info)
{
	if (!PHYS_OBJ_HAS_RIGIDBODY(obj0)) { return; }


	// move the objects out of one another
	phys_collision_response_resolve_position(obj0, obj1, info);

	// adjust velocity accordingly
	phys_collision_response_resolve_velocity(obj0, obj1, info);

}

void phys_collision_response_resolve_position(phys_obj_t* obj0, phys_obj_t* obj1, collision_info_t info)
{
	// magnitude of vec3, didnt find a glm function
	// f32 dist = sqrtf((info.direction[0] * info.direction[0]) + 
	// 				 (info.direction[1] * info.direction[1]) + 
	// 				 (info.direction[2] * info.direction[2]));

	// const f32 scalar = 0.005f; // @TODO: should not have to do this
	if (PHYS_OBJ_HAS_RIGIDBODY(obj0) && PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// move back according to mass
		f32 s1_ratio = 0;
		f32 s2_ratio = 0;

		if (obj0->rb.mass > obj1->rb.mass)
		{
			f32 ratio = obj1->rb.mass / obj0->rb.mass;
			s1_ratio = ratio;
			s2_ratio = 1 - ratio;
		}
		else if (obj0->rb.mass < obj1->rb.mass)
		{
			// f32 ratio = obj1->rb.mass / obj1->rb.mass;
			// s1_ratio = 1 - ratio;
			// s2_ratio = ratio;
      // @NOTE: above is always 0 & 1
      s1_ratio = 0;
      s2_ratio = 1;
		}
		else if (obj0->rb.mass == obj1->rb.mass)
		{
			s1_ratio = 0.5f;
			s2_ratio = 0.5f;
		}
		// f32 s1_dist = dist * s1_ratio;
		// vec3 offset;
		// vec3 s1_dist_vec = { s1_dist, s1_dist, s1_dist };
		// vec3_mul(info.normal, s1_dist_vec, offset);

		vec3_add(info.direction, obj0->pos, obj0->pos);

		// vec3 norm_inv;
		// vec3_copy(info.direction, norm_inv);
		// vec3_negate(norm_inv);
		// vec3_mul_f(norm_inv, scalar);
		// vec3_add(norm_inv, e1->pos, e1->pos);

		// f32 s2_dist = dist * s2_ratio;
		// vec3 s2_dist_vec = { s2_dist, s2_dist, s2_dist };
		// vec3_mul(info.normal, s2_dist_vec, offset);

		vec3 norm_inv;
		vec3_copy(info.direction, norm_inv);
		vec3_negate(norm_inv, norm_inv);
		vec3_add(norm_inv, obj1->pos, obj1->pos);

		// vec3_add(info.direction, e2->pos, e2->pos);

	}
	else if (!PHYS_OBJ_HAS_RIGIDBODY(obj0) && PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// only move back the rb
		// vec3 offset;
		// vec3 norm_inv;
		// vec3_copy(info.normal, norm_inv);
		// vec3_negate(norm_inv);
		// vec3 dist_vec = { dist, dist, dist };
		// vec3_mul(info.normal, dist_vec, offset);
		// vec3_add(offset, e2->pos, e2->pos);

		vec3_add(info.direction, obj1->pos, obj1->pos);

		vec3 norm_inv;
		vec3_copy(info.direction, norm_inv);
		vec3_negate(norm_inv, norm_inv);
		vec3_add(norm_inv, obj1->pos, obj1->pos);
	}
	else if (PHYS_OBJ_HAS_RIGIDBODY(obj0) && !PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// only move back the rb
		// vec3 offset;
		// vec3 dist_vec = { dist, dist, dist };
		// vec3_mul(info.normal, dist_vec, offset);
		// vec3_add(offset, e1->pos, e1->pos);

		// vec3 norm_inv;
		// vec3_copy(info.direction, norm_inv);
		// vec3_negate(norm_inv);
		// vec3_mul_f(norm_inv, scalar);
		// vec3_add(norm_inv, e1->pos, e1->pos);

		vec3_add(info.direction, obj0->pos, obj0->pos);
	}
}

void phys_collision_response_resolve_velocity(phys_obj_t* obj0, phys_obj_t* obj1, collision_info_t info)
{
	// calc the force each collider enacted on the other

	// fill the vars according to rb or static
	f32  mass1 = 0;
	f32  mass2 = 0;
	vec3 velocity1;
	vec3 velocity2;
	if (PHYS_OBJ_HAS_RIGIDBODY(obj0))
	{
		mass1 = obj0->rb.mass;
		vec3_copy(obj0->rb.velocity, velocity1);
	}
	else // static
	{
		mass1 = obj0->rb.mass;
		vec3_copy(VEC3(0), velocity1);
	}
	if (PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		mass2 = obj1->rb.mass;
		vec3_copy(obj1->rb.velocity, velocity2);
	}
	else // static
	{
		mass2 = mass1;
		vec3_copy(VEC3(0), velocity2);
	}
	// printf("mass1: %.2f, mass2: %.2f\n", mass1, mass2);
	// printf("v1: x: %.2f, y: %.2f, z: %.2f\n", velocity1[0], velocity1[1], velocity1[2]);
	// printf("v2: x: %.2f, y: %.2f, z: %.2f\n", velocity2[0], velocity2[1], velocity2[2]);


	// see: elastic collisions, one-dimensional newtonian

	// velocity1 formula
	// v1,2 = current velocity, m1,2 = mass
	// ((m1 - m2) / (m1 + m2)) * v1
	// +
	// ((2 * m2) / (m1 + m2)) * v2
	f32 m1_m_m2 = mass1 - mass2;
	f32 m1_p_m2 = mass1 + mass2;
	f32 a1_f = (m1_m_m2 / m1_p_m2);
	vec3 a1 = { a1_f, a1_f, a1_f };
	vec3_mul(velocity1, a1, a1);
	f32 b1_f = (2 * mass2) / m1_p_m2;
	vec3 b1 = { b1_f, b1_f, b1_f };
	vec3_mul(velocity2, b1, b1);

	vec3 v1;
	vec3_add(a1, b1, v1);

	// velocity2 formula
	// ((2 * m1) / (m1 + m2)) * v1
	// +
	// ((m2 - m1) / (m1 + m2)) * v2
	f32 a2_f = (2 * mass1) / m1_p_m2;
	vec3 a2 = { a2_f, a2_f, a2_f };
  vec3_mul(velocity1, a2, a2);
	f32 b2_f = (mass2 - mass1) / m1_p_m2;
	vec3 b2 = { b2_f, b2_f, b2_f };
	vec3_mul(velocity2, b2, b2);

	vec3 v2;
	vec3_add(a2, b2, v2);

	// set velocity
	if (PHYS_OBJ_HAS_RIGIDBODY(obj0) && !PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// vec3_copy(v1, e1->rb.velocity);

	  vec3_copy(info.direction, v1);
		vec3_mul_f(v1, 10, v1);
		vec3_copy(v1, obj0->rb.velocity);
		// printf("s1 v: x: %.2f, y: %.2f, z: %.2f\n", e1->rb.velocity[0], e1->rb.velocity[1], e1->rb.velocity[2]);
	}
  else if (!PHYS_OBJ_HAS_RIGIDBODY(obj0) && PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// vec3_copy(v2, e2->rb.velocity);

		vec3_copy(info.direction, v2);
		vec3_mul_f(v2, 10, v2);
		vec3_copy(v2, obj1->rb.velocity);
		// printf("s2 v: x: %.2f, y: %.2f, z: %.2f\n", e2->rb.velocity[0], e2->rb.velocity[1], e2->rb.velocity[2]);
	}
  else if (PHYS_OBJ_HAS_RIGIDBODY(obj0) && PHYS_OBJ_HAS_RIGIDBODY(obj1))
	{
		// vec3_add(v1, e1->rb.velocity, e1->rb.velocity);
		// vec3_add(v2, e2->rb.velocity, e2->rb.velocity);


		vec3_copy(info.direction, v1);

		vec3 dir;
		vec3_copy(info.direction, dir);
		vec3_normalize(dir, dir);

		vec3_mul(v1, dir, v1);
		// vec3_mul_f(v1, 10);
		vec3_copy(v1, obj0->rb.velocity);


		vec3_copy(info.direction, v2);

		vec3_negate(dir, dir);
		vec3_mul(v2, dir, v2);
		// vec3_mul_f(v2, 10);
		// vec3_negate(v2); // ???
		vec3_copy(v2, obj1->rb.velocity);

		// printf("rb1: \"%s\", v: x: %.2f, y: %.2f, z: %.2f\n", e1->name, e1->rb.velocity[0], e1->rb.velocity[1], e1->rb.velocity[2]);
		// printf("rb2: \"%s\", v: x: %.2f, y: %.2f, z: %.2f\n", e2->name, e2->rb.velocity[0], e2->rb.velocity[1], e2->rb.velocity[2]);
	}
}
