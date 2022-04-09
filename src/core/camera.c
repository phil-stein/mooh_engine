#include "core/camera.h"

#define GLFW_INCLUDE_NONE
#include "GLFW\glfw3.h"

#include "global/global.h"
#include "math/math_inc.h"


// ---- vars ----
// vec3 position = { 0.0f,   6.0f,  10.0f };
// vec3 front	  = { 0.0f,  -0.15f, -1.0f };
vec3 position = { 0.0f,   .0f,  0.0f };
vec3 front	  = { 0.0f,  0.0f, -1.0f };
vec3 up		    = { 0.0f,   1.0f,   0.0f };

vec3 target   = { 0.0, 0.0, 0.0 };

const float near_plane = 0.1f;
const float far_plane  = 1000.0f; // 100.0f;
const float fov        = 45.0f;
const float fov_rad    = 45.0f * M_PI_F / 180.0f; 

// editor camera -------------------------------------

// ---- func ----
void camera_move(vec3 dist)
{
	vec3_add(position, dist, position);
}

// ---- set ----
void camera_set_pos(vec3 pos)
{
	vec3_copy(pos, position);
}
void camera_set_front(vec3 dir)
{
	vec3_copy(dir, front);
}
void camera_set_up(vec3 dir)
{
	vec3_copy(dir, up);
}

// ---- get ----
void camera_get_pos(vec3 pos)
{
	vec3_copy(position, pos);
}
void camera_get_front(vec3 dir)
{
	vec3_copy(front, dir);
}
void camera_get_up(vec3 dir)
{
	vec3_copy(up, dir);
}
float camera_get_n_plane()
{ return near_plane; }
float camera_get_f_plane()
{ return far_plane; }
float camera_get_fov()
{ return fov; }

void camera_get_inv_direction(vec3 dir)
{
	// same as: dir = position - target; 
	vec3_sub(position, target, dir);
	vec3_normalize(dir, dir);
}

void camera_get_right_axis(vec3 axis)
{
	vec3 up = { 0.0, 1.0, 0.0 };
	vec3 dir = { 0.0, 0.0, 0.0 };
	camera_get_inv_direction(dir);

	// axis = { 0.0, 0.0, 0.0 };
	vec3_cross(up, dir, axis);
	vec3_normalize(axis, axis);
}

void camera_get_up_axis(vec3 axis)
{
	vec3 dir = { 0.0, 0.0, 0.0 }; 
	camera_get_inv_direction(dir);

	vec3 right = { 0.0, 0.0, 0.0 }; 
	camera_get_right_axis(right);

	vec3_cross(dir, right, axis);
}

void camera_get_turntable_view_mat(const float radius, mat4 view)
{
	f32 cam_x = (f32)sin(glfwGetTime()) * radius;
	f32 cam_z = (f32)cos(glfwGetTime()) * radius;

	vec3 pos = { cam_x, 0.0, cam_z };
	vec3 center = { 0.0, 0.0, 0.0 };
	vec3 up = { 0.0, 1.0, 0.0 };
	mat4_lookat(pos, center, up, view);
}

void camera_get_view_mat(mat4 view)
{
	vec3 center;
	vec3_add(position, front, center);
	mat4_lookat(position, center, up, view);
}
void camera_get_proj_mat(int width, int height, mat4 proj)
{
  mat4_perspective(fov_rad, ((f32)width / (f32)height), near_plane, far_plane, proj);
}

