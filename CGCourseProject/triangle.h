#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include <stdint.h>
#include "texture.h"

typedef struct
{
	vec4_t points[3];
	tex2_t uv_coordinate[3];
	uint32_t color;
	upng_t* texture;
}triangle_t;

typedef struct {
	int a, b, c;

	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;

	uint32_t color;
} face_t;


vec3_t get_face_normal(vec4_t face_points[3]);

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_textured_triangle(
	vec4_t a, tex2_t uv0,
	vec4_t b, tex2_t uv1,
	vec4_t c, tex2_t uv2,
	upng_t* texture);

void fill_flat_button_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

vec3_t barycentric_coordinate(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

void draw_texel(
	upng_t* texture,
	vec2_t point_p,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t uv0,
	tex2_t uv1,
	tex2_t uv2);


void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

// void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

//void draw_filled_triangle(
//	int x0, int y0, float z0, float w0,
//	int x1, int y1, float z1, float w1,
//	int x2, int y2, float z2, float w2,
//	uint32_t color
//);

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
);

void draw_triangle_pixel(
	int x, int y, uint32_t color,
	vec4_t point_a, vec4_t point_b, vec4_t point_c
);

#endif // !TRIANGLE_H


