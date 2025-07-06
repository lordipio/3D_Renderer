#include "light.h"

static light_t light;

void init_light(vec3_t direction)
{
	light.direction = direction;
}

vec3_t get_light_direction()
{
	return light.direction;
}


uint32_t light_apply_intensity(uint32_t original_color, float percentage_color)
{
	if (percentage_color < 0.1) percentage_color = 0.1;
	if (percentage_color > 1) percentage_color = 1;

	uint32_t a = (original_color & 0xFF000000);
	uint32_t r = (original_color & 0x00FF0000) * percentage_color;
	uint32_t g = (original_color & 0x0000FF00) * percentage_color;
	uint32_t b = (original_color & 0x000000FF) * percentage_color;

	uint32_t face_final_color = (a & 0xFF000000) | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

	return face_final_color;
}
