#ifndef CAMERA_H
#define CAMERA_H

#include <global/global.h>
#include <math/math_inc.h>

// ---- editor camera ----

void camera_init();

// add to the current position
void camera_move(vec3 dist);

void camera_set_pos(vec3 pos);
void camera_set_front(vec3 dir);
void camera_set_up(vec3 dir);


// get the cameras current position
void camera_get_pos(vec3 pos);

// get the cameras current front direction
void camera_get_front(vec3 dir);

// get the cameras current up direction
void camera_get_up(vec3 dir);

// float camera_get_n_plane();
// float camera_get_f_plane();
// float camera_get_fov();

// get the direction from the taget towards the camera (normalized)
void camera_get_inv_direction(vec3 dir);

// get the cameras right axis (normalized)
void camera_get_right_axis(vec3 axis);

// get the cameras up axis (normalized)
void camera_get_up_axis(vec3 axis);

void camera_get_turntable_view_mat(const float radius, mat4 dest);

// normal view matrix
void camera_get_view_mat(mat4 view);

// projection matrix
void camera_get_proj_mat(int width, int height, mat4 proj);

#endif
