#ifndef CORE_CAMERA_H
#define CORE_CAMERA_H

#include "global/global.h"
#include "math/math_inc.h"


// @DOC: // initialize the camera, call this before any other calls to cam_...()
void camera_init();

// @DOC: add to the current position
void camera_move(vec3 dist);
// @DOC: override the cams position
void camera_set_pos(vec3 pos);
// @DOC: override the cams front vector, aka. forward dir
void camera_set_front(vec3 dir);
// @DOC: override the cams up vector
void camera_set_up(vec3 dir);


// // @DOC: get the cameras current position
// void camera_get_pos(vec3 pos);
// 
// // @DOC: get the cameras current front direction
// void camera_get_front(vec3 dir);
// 
// // @DOC: get the cameras current up direction
// void camera_get_up(vec3 dir);
// 
// float camera_get_n_plane();
// float camera_get_f_plane();
// float camera_get_fov();

// @DOC: get the direction from the taget towards the camera (normalized)
void camera_get_inv_direction(vec3 dir);

// @DOC: get the cameras right axis (normalized)
void camera_get_right_axis(vec3 axis);

// @DOC: get the cameras up axis (normalized)
void camera_get_up_axis(vec3 axis);

// @DOC: get view mat for a turntable style camera 
void camera_get_turntable_view_mat(const float radius, mat4 dest);

// @DOC: get cam normal view matrix
void camera_get_view_mat(mat4 view);

// @DOC: get cam projection matrix
void camera_get_proj_mat(int width, int height, mat4 proj);

#endif
