#include "matrix.h"
#include <math.h>
#include <stdio.h>

mat4_t mat4_identity(void)
{
	mat4_t m =
	{ {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	} };

	return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
	mat4_t m = mat4_identity();
	
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;

	return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
	mat4_t m = mat4_identity();

	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;

	return m;
}

mat4_t mat4_make_rotation_x(float angle)
{
	// |   1    0    0    0 |
	// |   0   cos -sin   0 |
	// |   0   sin  cos   1 |
	// |   0    0    0    1 |

	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	mat4_t result = mat4_identity();

	result.m[1][1] =  cos_angle;
	result.m[1][2] = -sin_angle;
	result.m[2][1] =  sin_angle;
	result.m[2][2] =  cos_angle;

	return result;
	
}

mat4_t mat4_make_rotation_y(float angle)
{
	// |  cos   0    sin  0 |
	// |   0    1    0    0 |
	// | -sin   0    cos  0 |
	// |   0    0    0    1 |

	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	mat4_t result = mat4_identity();

	result.m[0][0] =  cos_angle;
	result.m[0][2] =  sin_angle;
	result.m[2][0] = -sin_angle;
	result.m[2][2] =  cos_angle;

	return result;

}

mat4_t mat4_make_rotation_z(float angle)
{
	// |  cos -sin   0    0 |
	// |  sin  cos   0    0 |
	// |   0    0    1    0 |
    // |   0    0    0    1 |

	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	mat4_t result = mat4_identity();

	result.m[0][0] =  cos_angle;
	result.m[0][1] = -sin_angle;
	result.m[1][0] =  sin_angle;
	result.m[1][1] =  cos_angle;

	return result;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
	vec4_t result;

	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

	return result;
}


mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2)
{
	mat4_t result;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] +m1.m[i][3] * m2.m[3][j];
		}

	return result;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
	// | aspect * (1/tan(fov/2))    	0			     0					0		 |
	// | 	     0                (1/tan(fov/2))	     0					0		 |
	// |         0    				    0	          lambda		- znear * lambda |
	// |		 0    					0				1					0		 |


	float f = 1 / tan(fov / 2);

	float lambda = zfar / (zfar - znear);

	mat4_t result = { { 0 } };
	
	result.m[0][0] = aspect * f;
	result.m[1][1] = f;
	result.m[2][2] = lambda;
	result.m[2][3] = - znear * lambda;
	result.m[3][2] = 1;

	return result;
}

mat4_t mat4_make_look_at(vec3_t eye, vec3_t target, vec3_t up)
{
	vec3_t forward = vec3_sub(target, eye);
	vec3_normalize(&forward);

	vec3_t right = vec3_cross(up, forward);
	vec3_normalize(&right);

	vec3_t relative_up = vec3_cross(forward, right);

	mat4_t view_matrix = 
	{{
		{right.x, right.y, right.z, -vec3_dot(right, eye)},
		{relative_up.x, relative_up.y, relative_up.z, -vec3_dot(relative_up, eye)},
		{forward.x, forward.y, forward.z, -vec3_dot(forward, eye)},
		{0, 0, 0, 1}
	}};

	return view_matrix;
}

mat4_t mat4_make_perspective_c(float fov, float aspect, float znear, float zfar) {
	// | (h/w)*1/tan(fov/2)             0              0                 0 |
	// |                  0  1/tan(fov/2)              0                 0 |
	// |                  0             0     zf/(zf-zn)  (-zf*zn)/(zf-zn) |
	// |                  0             0              1                 0 |
	mat4_t m = { {{ 0 }} };
	m.m[0][0] = aspect * (1 / tan(fov / 2));
	m.m[1][1] = 1 / tan(fov / 2);
	m.m[2][2] = zfar / (zfar - znear);
	m.m[2][3] = (-zfar * znear) / (zfar - znear);
	m.m[3][2] = 1.0;
	return m;
}

vec4_t mat4_to_ndc(vec4_t point)
{
	if (point.w != 0)
	{
		point.x /= point.w;
		point.y /= point.w;
		point.z /= point.w;
	}

	return point;
}


vec4_t mat4_to_screen_space(vec4_t point, int width, int height)
{
	vec4_t result = { point.x, point.y };

	// scale
	result.x *= width / 2.f;
	result.y *= height / 2.f;

	result.y *= -1;

	// translation
	result.x += width / 2.f;
	result.y += height / 2.f;


	result.z = point.z;
	result.w = point.w;

	return result;
}


vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
	// multiply the projection matrix by our original vector
	vec4_t result = mat4_mul_vec4(mat_proj, v);

	// perform perspective divide with original z-value that is now stored in w
	if (result.w != 0.0) {
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
	}
	return result;
}