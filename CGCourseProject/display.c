#include "display.h"

static int window_width;
static int window_height;
static float pixelated_factor = 1.f;
static float* depth_buffer = NULL;
static uint32_t* color_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static SDL_Window* window;
static SDL_Renderer* renderer;

static int cull_method = 0;
static int render_method = 0;

int get_width(void)
{
	return window_width;
}

int get_height(void)
{
	return window_height;
}

void set_render_method(int method)
{
	render_method = method;
}

void set_cull_method(int method)
{
	cull_method = method;
}

bool is_cull_backface(void)
{
	return (cull_method == CULL_BACKFACE);
}

bool should_render_filled_triangles(void)
{
	return (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE);
}

bool should_render_textured_triangles(void)
{
	return (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE);
}

bool should_render_wireframe(void)
{
	return (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE);
}

bool should_render_vertices(void)
{
	return (render_method == RENDER_WIRE_VERTEX);
}

bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL!");
		return false;
	}

	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

	int full_window_width = display_mode.w;
	int full_window_height = display_mode.h;

	//window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, full_window_width, full_window_height, SDL_WINDOW_BORDERLESS);
	window = SDL_CreateWindow("CG Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, full_window_width * 0.8, full_window_height * 0.8, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	window_width = full_window_width / pixelated_factor;
	window_height = full_window_height / pixelated_factor;


	if (!window)
	{
		fprintf(stderr, "Error creating window!");
		return false;
	}
	

	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer)
	{
		fprintf(stderr, "Error creating renderer!");
		return false;
	}

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);


	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

	depth_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

	clear_depth_buffer();

	return true;
}

void draw_grid(void)
{
	for (int y = 0; y < window_width; y += 10)
		for (int x = 0; x < window_height; x += 10)
		{
			// if (x % 10 == 0 || y % 10 == 0)
			color_buffer[y + x * window_width] = 0xFFEEEEEE;
		}
}

void draw_sky(void) {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a = 255;
	uint32_t color;

	for (int y = 0; y < window_height; y++) {
		float t = (float)y / (float)(window_height - 1);

		r = (uint8_t)((1 - t) * 135 + t * 0);
		g = (uint8_t)((1 - t) * 206 + t * 0);
		b = (uint8_t)((1 - t) * 250 + t * 64);

		color = (a << 24) | (b << 16) | (g << 8) | r;

		for (int x = 0; x < window_width; x++) {
			draw_pixel(x, y, color);
		}
	}
}

void draw_rect(int x_pos, int y_pos, int width, int height, uint32_t color)
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			int current_x = x + x_pos;
			int current_y = y + y_pos;

			draw_pixel(current_x, current_y, color);
			// color_buffer[current_y * window_width + current_x] = color;
		}
}


void render_color_buffer(void)
{

	SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)sizeof(uint32_t) * window_width);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}

void set_depth_buffer_at(int x, int y, float depth_value)
{
	depth_buffer[y * window_width + x] = depth_value;
}

float get_depth_buffer_at(int x, int y)
{
	return depth_buffer[y * window_width + x];
}

void clear_depth_and_color_buffer(uint32_t color) {
	int limit = window_height * window_width;
	for (int i = 0; i < limit; i++)
	{
			color_buffer[i] = color;
			depth_buffer[i] = 1.f;

	}
}

void clear_depth_buffer(void) {
	int limit = window_height * window_width;
	for (int i = 0; i < limit; i++)
		depth_buffer[i] = 1.f;
}

void destroy_window(void)
{
	free(depth_buffer);
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x >= 0 && x < window_width && y < window_height && y >= 0)
		color_buffer[y * window_width + x] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;

	int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i < longest_side_length; i++)
	{
		draw_pixel(round(current_x), round(current_y), color);

		current_x += x_inc;
		current_y += y_inc;
	}
}


