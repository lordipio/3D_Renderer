#include "triangle.h"
#include "display.h"
#include "swap.h"


vec3_t barycentric_coordinate(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t pc = vec2_sub(c, p);
	vec2_t pb = vec2_sub(b, p);

	float area_parallelogram_abc = ac.x * ab.y - ac.y * ab.x;

	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
	float gamma = 1 - beta - alpha;

	vec3_t weights = { alpha, beta, gamma };
	return weights;
}

void draw_texel(
	upng_t* texture,
	vec2_t point_p,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t uv0,
	tex2_t uv1,
	tex2_t uv2)
{

	vec2_t p = point_p;
	vec2_t a = vec4_to_vec2(point_a);
	vec2_t b = vec4_to_vec2(point_b);
	vec2_t c = vec4_to_vec2(point_c);

	if (p.x < 0 || p.x >= get_width() || p.y < 0 || p.y >= get_height())
	{
		return;
	}
	vec3_t weights = barycentric_coordinate(a, b, c, p);

	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);


	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u = alpha * uv0.u / point_a.w + beta * uv1.u / point_b.w + gamma * uv2.u / point_c.w;
	float interpolated_v = alpha * uv0.v / point_a.w + beta * uv1.v / point_b.w + gamma * uv2.v / point_c.w;

	float interpolated_reciprocal_w = alpha / point_a.w + beta / point_b.w + gamma / point_c.w;

	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	interpolated_reciprocal_w = 1 - interpolated_reciprocal_w;


	if (interpolated_reciprocal_w < get_depth_buffer_at(p.x, p.y))
	{
		uint32_t* texture_buffer = upng_get_buffer(texture);

		draw_pixel(p.x, p.y, texture_buffer[tex_x + tex_y * upng_get_width(texture)]);

		set_depth_buffer_at(p.x, p.y, interpolated_reciprocal_w);
	}

}

void draw_triangle_pixel(
	int x, int y, uint32_t color,
	vec4_t point_a, vec4_t point_b, vec4_t point_c
) {
	vec2_t p = { x, y };
	vec2_t a = vec4_to_vec2(point_a);
	vec2_t b = vec4_to_vec2(point_b);
	vec2_t c = vec4_to_vec2(point_c);

	if (p.x < 0 || p.x >= get_width() || p.y < 0 || p.y >= get_height())
		return;

	vec3_t weights = barycentric_coordinate(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	if (interpolated_reciprocal_w < get_depth_buffer_at(x, y)) {
		draw_pixel(x, y, color);
		set_depth_buffer_at(p.x, p.y, interpolated_reciprocal_w);
	}
}

void fill_flat_button_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	float x0_x1_slop = (float)(x1 - x0) / (y1 - y0);
	float x0_x2_slop = (float)(x2 - x0) / (y2 - y0);

	float x_start = x0;
	float x_end = x0;

	for (int y = y0; y <= y2; y++)
	{
		draw_line(x_start, y, x_end, y, color);

		x_start += x0_x1_slop;
		x_end += x0_x2_slop;
	}
}


void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	float x0_x1_slop = (float)(x1 - x2) / (y1 - y2);
	float x0_x2_slop = (float)(x0 - x2) / (y0 - y2);

	float x_start = x2;
	float x_end = x2;

	for (int y = y2; y >= y0; y--)
	{
		draw_line(x_start, y, x_end, y, color);

		x_start -= x0_x1_slop;
		x_end -= x0_x2_slop;
	}
}

void draw_textured_triangle(
	vec4_t a, tex2_t uv0,
	vec4_t b, tex2_t uv1,
	vec4_t c, tex2_t uv2,
	upng_t* texture)
{
	int y0 = a.y;
	int y1 = b.y;
	int y2 = c.y;

	int x0 = a.x;
	int x1 = b.x;
	int x2 = c.x;

	float z0 = a.z;
	float z1 = b.z;
	float z2 = c.z;

	float w0 = a.w;
	float w1 = b.w;
	float w2 = c.w;

	float u0 = uv0.u;
	float u1 = uv1.u;
	float u2 = uv2.u;

	float v0 =  1 - uv0.v;
	float v1 =  1 - uv1.v;
	float v2 =  1 - uv2.v;

	if (y0 > y1)
	{
		swap_int(&y0, &y1);
		swap_int(&x0, &x1);
		swap_float(&z0, &z1);
		swap_float(&w0, &w1);

		swap_float(&u0, &u1);
		swap_float(&v0, &v1);
	}

	if (y1 > y2)
	{
		swap_int(&y2, &y1);
		swap_int(&x2, &x1);
		swap_float(&z2, &z1);
		swap_float(&w2, &w1);

		swap_float(&u2, &u1);
		swap_float(&v2, &v1);
	}

	if (y0 > y1)
	{
		swap_int(&y0, &y1);
		swap_int(&x0, &x1);
		swap_float(&z0, &z1);
		swap_float(&w0, &w1);
		swap_float(&u0, &u1);
		swap_float(&v0, &v1);
	}

	float inverse_slop1;
	float inverse_slop2;
	int x_start = x0;
	int x_end = x0;


	if (y1 - y0 != 0) inverse_slop1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inverse_slop2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0)
		for (int y = y0; y <= y1; y++)
		{
			x_start = x0 + (y - y0) * inverse_slop1;
			x_end = x0 + (y - y0) * inverse_slop2;

			if (x_start > x_end)
				swap_int(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				vec2_t point_p = { .x = x, .y = y };
				vec4_t point_a = { x0, y0, z0, w0 };
				vec4_t point_b = { x1, y1, z1, w1 };
				vec4_t point_c = { x2, y2, z2, w2 };

				uv0.u = u0;
				uv0.v = v0;

				uv1.u = u1;
				uv1.v = v1;

				uv2.u = u2;
				uv2.v = v2;

				draw_texel(texture, point_p, point_a, point_b, point_c, uv0, uv1, uv2);
			}
		}



	if (y1 - y2 != 0) inverse_slop1 = (float)(x1 - x2) / abs(y1 - y2);
	if (y0 - y2 != 0) inverse_slop2 = (float)(x0 - x2) / abs(y0 - y2);

	if (y1 - y2 != 0)
		for (int y = y2; y >= y1; y--)
		{
			x_start = x2 + (-y + y2) * inverse_slop1;
			x_end = x2 + (-y + y2) * inverse_slop2;

			if (x_start > x_end)
				swap_int(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				vec2_t point_p = { .x = x, .y = y };
				vec4_t point_a = { x0, y0, z0, w0 };
				vec4_t point_b = { x1, y1, z1, w1 };
				vec4_t point_c = { x2, y2, z2, w2 };

				uv0.u = u0;
				uv0.v = v0;

				uv1.u = u1;
				uv1.v = v1;

				uv2.u = u2;
				uv2.v = v2;

				draw_texel(texture, point_p, point_a, point_b, point_c, uv0, uv1, uv2);
			}
		}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
) {
	if (y0 > y1) {
		swap_int(&y0, &y1);
		swap_int(&x0, &x1);
		swap_float(&z0, &z1);
		swap_float(&w0, &w1);
	}
	if (y1 > y2) {
		swap_int(&y1, &y2);
		swap_int(&x1, &x2);
		swap_float(&z1, &z2);
		swap_float(&w1, &w2);
	}
	if (y0 > y1) {
		swap_int(&y0, &y1);
		swap_int(&x0, &x1);
		swap_float(&z0, &z1);
		swap_float(&w0, &w1);
	}

	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };

	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				swap_int(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
			}
		}
	}

	inv_slope_1 = 0;
	inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				swap_int(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
			}
		}
	}
}

vec3_t get_face_normal(vec4_t vertices[3])
{
	vec3_t b_a = vec3_sub(vec4_to_vec3(vertices[1]), vec4_to_vec3(vertices[0]));
	vec3_t c_a = vec3_sub(vec4_to_vec3(vertices[2]), vec4_to_vec3(vertices[0]));

	vec3_t normal = vec3_cross(b_a, c_a);
	vec3_normalize(&normal);

	return normal;
}
