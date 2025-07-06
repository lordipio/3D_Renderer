#include "camera.h"
#include "matrix.h"

static camera_t camera;

void init_camera(vec3_t position, vec3_t direction, vec3_t forward_velocity, float yaw, float pitch)
{
	camera.position = position;
	camera.direction = direction;
	camera.forward_velocity = forward_velocity;
	camera.yaw = yaw;
	camera.pitch = pitch;
}


vec3_t get_camera_position()
{
	return camera.position;
}

vec3_t get_camera_direction()
{
	return camera.direction;
}

vec3_t get_camera_forward_velocity()
{
	return camera.forward_velocity;
}

float get_camera_yaw()
{
	return camera.yaw;
}

float get_camera_pitch()
{
	return camera.pitch;
}

void set_camera_position(vec3_t position)
{
	camera.position = position;
}

void set_camera_direction(vec3_t direction)
{
	camera.direction = direction;
}

void set_camera_forward_velocity(vec3_t forward_velocity)
{
	camera.forward_velocity = forward_velocity;
}

void add_camera_yaw(float yaw)
{
	camera.yaw += yaw;
}

void add_camera_pitch(float pitch)
{
	camera.pitch += pitch;
}

vec3_t get_camera_target_look_at(void)
{
	vec3_t target = { 0, 0, 1 };

	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

	mat4_t camera_rotation = mat4_identity();
	camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
	camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

	vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec3_to_vec4(target));
	camera.direction = vec4_to_vec3(camera_direction);
	
	target = vec3_add(camera.position, camera.direction);

	return target;
}

vec3_t get_camera_right_vector(void)
{
	vec3_t up = { 0, 1, 0 };
	vec3_t right = vec3_cross(up, camera.direction); 
	vec3_normalize(&right);
	return right;
}
