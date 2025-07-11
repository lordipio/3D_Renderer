#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
typedef struct {
	float m[4][4];
}mat4_t;


mat4_t mat4_identity(void);

mat4_t mat4_make_scale(float sx, float sy, float sz);

mat4_t mat4_make_translation(float tx, float ty, float tz);

mat4_t mat4_make_rotation_x(float angle);

mat4_t mat4_make_rotation_y(float angle);

mat4_t mat4_make_rotation_z(float angle);

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2);

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);

mat4_t mat4_make_look_at(vec3_t eye, vec3_t target, vec3_t up);

vec4_t mat4_to_ndc(vec4_t point);

vec4_t mat4_to_screen_space(vec4_t point, int width, int height);

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v);

mat4_t mat4_make_perspective_c(float fov, float aspect, float znear, float zfar);

#endif // !MATRIX_H
