#include "phys/phys_world.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_response.h"
#include "phys/phys_collision.h"
#include "phys/phys_debug_draw.h"
#include "core/debug/debug_draw.h"

#include "stb/stb_ds.h"



phys_obj_t* phys_objs = NULL;
u32 phys_objs_len = 0;


// @TODO: move below update()

void phys_obj_make_rb(f32 mass, phys_obj_t* obj)
{
  obj->flags |= PHYS_HAS_RIGIDBODY;
  obj->rb.mass = mass;
  vec3_copy(VEC3(0), obj->rb.velocity);
  vec3_copy(VEC3(0), obj->rb.force);
  // @TODO: set these through arguments
  obj->rb.restitution      = 1.0f;
  obj->rb.static_friction  = 0.0f;
  obj->rb.dynamic_friction = 0.0f;
}
void phys_obj_make_box(vec3 aabb[2], vec3 offset, phys_obj_t* obj)
{
  // ASSERT(!HAS_FLAG(obj->flags, PHYS_HAS_SPHERE));
  ASSERT(!PHYS_OBJ_HAS_COLLIDER(obj));
  obj->flags |= PHYS_HAS_BOX;
  
  obj->collider.type = PHYS_COLLIDER_BOX;
  
  vec3_copy(offset, obj->collider.offset);
  obj->collider.is_trigger = false;
  
  vec3_copy(aabb[0], obj->collider.box.aabb[0]);
  vec3_copy(aabb[1], obj->collider.box.aabb[1]);

  obj->collider.infos = NULL;
  obj->collider.infos_len = 0;
}

void phys_add_obj_rb(u32 entity_idx, vec3 pos, f32 mass)
{
  phys_obj_t obj = PHYS_OBJ_T_INIT();
  obj.entity_idx = entity_idx;
  vec3_copy(pos, obj.pos);
  vec3_copy(VEC3(1), obj.scl);

  phys_obj_make_rb(mass, &obj);

  arrput(phys_objs, obj);
  phys_objs_len++;
}
void phys_add_obj_box(u32 entity_idx, vec3 pos, vec3 scl, vec3 aabb[2], vec3 offset)
{
  phys_obj_t obj = PHYS_OBJ_T_INIT();
  obj.entity_idx = entity_idx;
  vec3_copy(pos, obj.pos);
  vec3_copy(scl, obj.scl);

  phys_obj_make_box(aabb, offset, &obj); 

  arrput(phys_objs, obj);
  phys_objs_len++;
}
void phys_add_obj_rb_box(u32 entity_idx, vec3 pos, vec3 scl, f32 mass, vec3 aabb[2], vec3 offset)
{
  phys_obj_t obj = PHYS_OBJ_T_INIT();
  obj.entity_idx = entity_idx;
  vec3_copy(pos, obj.pos);
  vec3_copy(scl, obj.scl);

  phys_obj_make_rb(mass, &obj);
  phys_obj_make_box(aabb, offset, &obj);

  arrput(phys_objs, obj);
  phys_objs_len++;
}

// @TODO: 
void phys_remove_obj()
{
}

phys_obj_t* phys_get_obj_arr(u32* len)
{
  *len = phys_objs_len;
  return phys_objs;
}

void phys_init()
{
	// ...
}

void phys_update(f32 dt)
{

	// go through all objs
  // skip objects that are static or not colliders
	for (u32 i = 0; i < phys_objs_len; ++i) // array of rigidbodies
	{
    phys_obj_t* obj0 = &phys_objs[i];
    
    phys_debug_draw_collider(obj0);

		// ---- dynamics ----
		if (!PHYS_OBJ_HAS_RIGIDBODY(obj0)) { continue; }
    phys_dynamics_simulate(obj0, dt);

		// // ---- collision ----
		if (!PHYS_OBJ_HAS_COLLIDER(obj0)) { continue; }
  
		// test with all other colliders
		for (int j = 0; j < phys_objs_len; ++j) // array of colliders
		{
			if (j == i) { continue; }
		  if (!PHYS_OBJ_HAS_COLLIDER(obj0)) { continue; }
      phys_obj_t* obj1 = &phys_objs[j];

			collision_info_t c = phys_collision_check(obj0, obj1);
      
      // if (c.collision)
      // {
      //   debug_draw_sphere_register(obj0->pos, 0.35f, RGB_F(1, 0, 1));
      //   // REMOVE_FLAG(obj0->flags, PHYS_HAS_RIGIDBODY);
      // }

		  // ---- collision response ----
		  if (c.collision)
		  {
		  	// notify objects of collision

		  	c.trigger = obj0->collider.is_trigger || obj1->collider.is_trigger;

		  	c.obj_idx = obj1->entity_idx;
		  	arrput(obj0->collider.infos, c);
		  	obj0->collider.infos_len++;

		  	c.obj_idx = obj0->entity_idx;
		  	arrput(obj1->collider.infos, c);
		  	obj1->collider.infos_len++;

		    if (!c.trigger && PHYS_OBJ_HAS_RIGIDBODY(obj0)) // no response on trigger collisions
		  	{
		  		// printf(" -> \"%s\" v. \"%s\"\n", e1->name, e2->name);
		  		// printf(" -> dir: %.2f, %.2f, %.2f\n", c.direction[0], c.direction[1], c.direction[2]);

		  		phys_collision_response(obj0, obj1, c);
		  	}
		  }

		}
	}
}
