#ifndef PHYS_DEBUG_DRAW_H
#define PHYS_DEBUG_DRAW_H

#ifdef PHYS_DEBUG

#include "phys/phys_types.h"

#define PHYS_DEBUG_COLLIDER_COLOR RGB_F(1, 0, 1)

void phys_debug_draw_velocity_func(phys_obj_t* obj);
void phys_debug_draw_collider_func(phys_obj_t* obj);
void phys_debug_draw_box_collider_func(phys_obj_t* obj);

#define phys_debug_draw_velocity(obj)      phys_debug_draw_velocity_func(obj)
#define phys_debug_draw_collider(obj)      phys_debug_draw_collider_func(obj)
#define phys_debug_draw_box_collider(obj)  phys_debug_draw_box_collider_func(obj)

#else

#define phys_debug_draw_velocity(obj)  

#endif

#endif
