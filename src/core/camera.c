#include "core/camera.h"
#include "core/core_data.h"

#define GLFW_INCLUDE_NONE
#include "GLFW\glfw3.h"

#include "global/global.h"
#include "math/math_inc.h"


// ---- vars ----
// vec3 position = { 0.0f,   6.0f,  10.0f };
// vec3 front	  = { 0.0f,  -0.15f, -1.0f };

// vec3 position = { 0.0f,   .0f,  0.0f };
// vec3 front	  = { 0.0f,  0.0f, -1.0f };
// vec3 up		    = { 0.0f,   1.0f,   0.0f };
// 
// vec3 target   = { 0.0, 0.0, 0.0 };
// 
// const float near_plane = 0.1f;
// const float far_plane  = 1000.0f; // 100.0f;
// const float fov        = 45.0f;
// const float fov_rad    = 45.0f * M_PI_F / 180.0f; 

static core_data_t* core_data = NULL;

// editor camera -------------------------------------

void camera_init()
{
  core_data = core_data_get();
}

// ---- func ----
void camera_move(vec3 dist)
{
	vec3_add(core_data->cam.pos, dist, core_data->cam.pos);
}

// ---- set ----
void camera_set_pos(vec3 pos)
{
	vec3_copy(pos, core_data->cam.pos);
}
void camera_set_front(vec3 dir)
{
	vec3_copy(dir, core_data->cam.front);
}
void camera_set_up(vec3 dir)
{
	vec3_copy(dir, core_data->cam.up);
}

// ---- get ----
// void camera_get_pos(vec3 pos)
// {
// 	vec3_copy(core_data->cam_pos, pos);
// }
// void camera_get_front(vec3 dir)
// {
// 	vec3_copy(core_data->cam_front, dir);
// }
// void camera_get_up(vec3 dir)
// {
// 	vec3_copy(core_data->cam_up, dir);
// }
// float camera_get_n_plane()
// { return core_data->near_plane; }
// float camera_get_f_plane()
// { return core_data->far_plane; }
// float camera_get_fov()
// { return core_data->cam_fov; }

void camera_get_inv_direction(vec3 dir)
{
	// same as: dir = position - target; 
	vec3_sub(core_data->cam.pos, core_data->cam.target, dir);
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
	vec3_add(core_data->cam.pos, core_data->cam.front, center);
	mat4_lookat(core_data->cam.pos, center, core_data->cam.up, view);
}
void camera_get_proj_mat(int width, int height, mat4 proj)
{
  mat4_perspective(core_data->cam.fov_rad, ((f32)width / (f32)height), core_data->cam.near_plane, core_data->cam.far_plane, proj);
}

