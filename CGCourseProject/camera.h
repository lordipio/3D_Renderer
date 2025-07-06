
#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct
{
	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw;
	float pitch;
}camera_t;

void init_camera(vec3_t position, vec3_t direction, vec3_t forward_velocity, float yaw, float pitch);

vec3_t get_camera_position();

vec3_t get_camera_direction();

vec3_t get_camera_forward_velocity();

float get_camera_yaw();

float get_camera_pitch();

void set_camera_position(vec3_t position);

void set_camera_direction(vec3_t direction);

void set_camera_forward_velocity(vec3_t forward_velocity);

void add_camera_yaw(float yaw);

void add_camera_pitch(float pitch);

vec3_t get_camera_target_look_at();

vec3_t get_camera_right_vector(void);
#endif // !CAMERA_H
