#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>

#define FPS 30
#define FRAME_TARGET_TIME 1000/FPS

enum cull_method
{
	CULL_NONE,
	CULL_BACKFACE
} ;

enum render_method
{
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
};

bool initialize_window(void);

void draw_grid(void);

void draw_rect(int rect_x, int rect_y, int width, int height, uint32_t color);

void render_color_buffer(void);

void clear_depth_and_color_buffer(uint32_t color);

void clear_depth_buffer(void);

void destroy_window(void);

void draw_pixel(int x, int y, uint32_t color);

void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

void set_render_method(int method);

void set_cull_method(int method);

bool is_cull_backface(void);

bool should_render_filled_triangles(void);

bool should_render_textured_triangles(void);

bool should_render_wireframe(void);

bool should_render_vertices(void);

float get_depth_buffer_at(int x, int y);

void set_depth_buffer_at(int x, int y, float depth_value);

int get_width(void);

int get_height(void);

void draw_sky(void);

#endif // !DISPLAY_H
