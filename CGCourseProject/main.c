#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "vector.h"
#include "clipping.h"
#include "display.h"
#include "matrix.h"
#include "light.h"
#include "camera.h"
#include "mesh.h"

bool is_running = false;
float delta_time;
// triangle_t* triangles_to_render = NULL;

float camera_speed = 2.f;

#define MAX_TRIANGLES_PER_MESH 20000

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];

int num_triangles_to_render = 0;

vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

mat4_t view_matrix;

int previous_frame_time = 0;

mat4_t perspective_matrix;

vec2_t project(vec3_t point)
{
	vec2_t projected_point = { .x = point.x * fov_factor / point.z + get_width() / 2, .y = point.y * fov_factor / point.z + get_height() / 2 };
	return projected_point;
}

void process_graphics_pipeline_stages(mesh_t* mesh)
{
	mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);

	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

	mat4_t world_matrix;

	// camera
	vec3_t up = { 0, 1, 0 };
	vec3_t target = get_camera_target_look_at();

	view_matrix = mat4_make_look_at(get_camera_position(), target, up);

	triangle_t triangle_to_render;

	int mesh_faces_count = array_length(mesh->faces);

	for (int i = 0; i < mesh_faces_count; i++)
	{
		face_t mesh_face = mesh->faces[i];
		vec3_t face_vertices[3];

		face_vertices[0] = mesh->vertices[mesh_face.a];
		face_vertices[1] = mesh->vertices[mesh_face.b];
		face_vertices[2] = mesh->vertices[mesh_face.c];

		vec4_t transformed_vertices[3];
		for (int j = 0; j < 3; j++)
		{
			transformed_vertices[j] = vec3_to_vec4(face_vertices[j]);
			world_matrix = mat4_identity();

			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			transformed_vertices[j] = mat4_mul_vec4(world_matrix, transformed_vertices[j]);
			transformed_vertices[j] = mat4_mul_vec4(view_matrix, transformed_vertices[j]);
		}

		vec3_t face_normal = get_face_normal(transformed_vertices);

		// Backface Culling
		if (is_cull_backface())
		{
			vec3_t camera_ray = vec3_sub(vec4_to_vec3(transformed_vertices[0]), vec3_new(0, 0, 0));

			if (vec3_dot(camera_ray, face_normal) > 0)
				continue;
		}

		// Painter Algorithm
		//float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.f;

		float ligth_intensity = -vec3_dot(face_normal, get_light_direction());

		mesh_face.color = light_apply_intensity(mesh_face.color, ligth_intensity);

		polygon_t polygon = create_polygon_from_triangle
		(
			vec4_to_vec3(transformed_vertices[0]), mesh_face.a_uv,
			vec4_to_vec3(transformed_vertices[1]), mesh_face.b_uv,
			vec4_to_vec3(transformed_vertices[2]), mesh_face.c_uv
		);

		clip_polygon(&polygon);

		triangle_t triangles_after_clipping[MAX_NUM_POLYGON_TRIANGLES];
		int triangles_after_clipping_num = 0;

		create_triangles_from_polygon(&polygon, triangles_after_clipping, &triangles_after_clipping_num);

		// loop on clipped trianlges
		for (int t = 0; t < triangles_after_clipping_num; t++)
		{
			for (int j = 0; j < 3; j++)
			{
				vec4_t projected_point = mat4_mul_vec4(perspective_matrix, triangles_after_clipping[t].points[j]);

				projected_point = mat4_to_ndc(projected_point);

				projected_point = mat4_to_screen_space(projected_point, get_width(), get_height());

				triangle_to_render.points[j] = projected_point;
				triangle_to_render.uv_coordinate[j] = triangles_after_clipping[t].uv_coordinate[j];


			}

			triangle_to_render.color = mesh_face.color;
			triangle_to_render.texture = mesh->texture;
			//triangle_to_render.avg_depth = avg_depth;

			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH)
				triangles_to_render[num_triangles_to_render++] = triangle_to_render;
		}

	}
}

void setup(void)
{
	float aspecty = (float)get_height() / (float)get_width();
	float aspectx = (float)get_width() / (float)get_height();

	float fovy = M_PI / 3; // 60 degree
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	
	float znear = 1.5;
	float zfar = 100;

	init_light(vec3_new(0, -1, 0));

	perspective_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

	init_frustum_planes(fovx, fovy, znear, zfar);

	load_mesh("assets/drone.obj", "assets/drone.png", vec3_new(0, 0, 0), vec3_new(1, 1, 1), vec3_new(0, 2, 8));
	load_mesh("assets/crab.obj", "assets/crab.png", vec3_new(0, 0, 0), vec3_new(1, 1, 1), vec3_new(0, -2, 8));
	//load_mesh("assets/runway.obj", "assets/runway.png", vec3_new(0, 0, 0), vec3_new(1, 1, 1), vec3_new(0, -4, 0));
}

void process_input(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			is_running = false;

		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			is_running = false;

		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_1: set_render_method(RENDER_WIRE_VERTEX); break;
			case SDLK_2: set_render_method(RENDER_WIRE); break;
			case SDLK_3: set_render_method(RENDER_FILL_TRIANGLE); break;
			case SDLK_4: set_render_method(RENDER_FILL_TRIANGLE_WIRE); break;
			case SDLK_5: set_render_method(RENDER_TEXTURED); break;
			case SDLK_6: set_render_method(RENDER_TEXTURED_WIRE); break;
			case SDLK_c: set_cull_method(CULL_BACKFACE); break;
			case SDLK_x: set_cull_method(CULL_NONE); break;
			}
		}
	}

	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	vec3_t pos = get_camera_position();
	vec3_t direction = get_camera_direction();
	vec3_t right = get_camera_right_vector();
	vec3_t forward = vec3_mul(direction, camera_speed * delta_time);

	if (keystate[SDL_SCANCODE_Q]) pos.y -= camera_speed * delta_time;
	if (keystate[SDL_SCANCODE_E]) pos.y += camera_speed * delta_time;
	if (keystate[SDL_SCANCODE_A]) { pos = vec3_add(vec3_mul(right, -camera_speed * delta_time), pos); }
	if (keystate[SDL_SCANCODE_D]) { pos = vec3_add(vec3_mul(right, camera_speed * delta_time), pos); }


	if (keystate[SDL_SCANCODE_W]) pos = vec3_add(pos, forward);
	if (keystate[SDL_SCANCODE_S]) pos = vec3_sub(pos, forward);

	set_camera_position(pos);

	if (keystate[SDL_SCANCODE_LEFT])  add_camera_yaw(-camera_speed * delta_time);
	if (keystate[SDL_SCANCODE_RIGHT]) add_camera_yaw(camera_speed * delta_time);
	if (keystate[SDL_SCANCODE_UP])    add_camera_pitch(-camera_speed * delta_time);
	if (keystate[SDL_SCANCODE_DOWN])  add_camera_pitch(camera_speed * delta_time);
}

void update(void)
{
	// while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		SDL_Delay(time_to_wait);

	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.f;

	previous_frame_time = SDL_GetTicks();

	num_triangles_to_render = 0;


	for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++)
	{
		mesh_t* mesh = get_mesh(mesh_index);

		//mesh->rotation.x += 0.5f * delta_time;
		if (mesh_index != 1)
			mesh->rotation.y += 0.5f * delta_time;
		//mesh->rotation.y += 0.5f * delta_time;

		//mesh.scale.x += 0.004f;
		//mesh.scale.y += 0.004f;
		//mesh.scale.z += 0.004f;

		//mesh.translation.z = 5.f;
		//mesh.translation.x += 0.01f;

		process_graphics_pipeline_stages(mesh);
	}

	// sort z for painter algorithm
	//triangle_t temp;
	//int triangles_count = array_length(triangles_to_render);
	//for (int i = 0; i < triangles_count; i++)
	//	for (int j = i; j < triangles_count; j++)
	//	{
	//		if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth)
	//		{
	//			temp = triangles_to_render[i];
	//			triangles_to_render[i] = triangles_to_render[j];
	//			triangles_to_render[j] = temp;
	//		}
	//	}
}

void render(void)
{
	clear_depth_and_color_buffer(0xFF444444);
	
	//draw_grid();
	//draw_sky();


	for (int i = 0; i < num_triangles_to_render; i++)
	{
		triangle_t triangle_to_render = triangles_to_render[i];

		if (should_render_filled_triangles())
			draw_filled_triangle(
				triangle_to_render.points[0].x, triangle_to_render.points[0].y, triangle_to_render.points[0].z, triangle_to_render.points[0].w, // vertex A
				triangle_to_render.points[1].x, triangle_to_render.points[1].y, triangle_to_render.points[1].z, triangle_to_render.points[1].w, // vertex B
				triangle_to_render.points[2].x, triangle_to_render.points[2].y, triangle_to_render.points[2].z, triangle_to_render.points[2].w, // vertex C
				triangle_to_render.color
			);
		// draw_filled_triangle(triangle_to_render.points[0].x, triangle_to_render.points[0].y, triangle_to_render.points[1].x, triangle_to_render.points[1].y, triangle_to_render.points[2].x, triangle_to_render.points[2].y, triangle_to_render.color);
		
		if (should_render_textured_triangles())
			draw_textured_triangle(
				triangle_to_render.points[0], triangle_to_render.uv_coordinate[0],
				triangle_to_render.points[1], triangle_to_render.uv_coordinate[1],
				triangle_to_render.points[2], triangle_to_render.uv_coordinate[2],
				triangle_to_render.texture
			);



		if (should_render_wireframe())
			draw_triangle(triangle_to_render.points[0].x, triangle_to_render.points[0].y, triangle_to_render.points[1].x, triangle_to_render.points[1].y, triangle_to_render.points[2].x, triangle_to_render.points[2].y, 0xFFFFFFFF);

		if (should_render_vertices())
		{
			draw_rect(triangle_to_render.points[0].x - 3, triangle_to_render.points[0].y - 3, 6, 6, 0xFFFF0000);
			draw_rect(triangle_to_render.points[1].x - 3, triangle_to_render.points[1].y - 3, 6, 6, 0xFFFF0000);
			draw_rect(triangle_to_render.points[2].x - 3, triangle_to_render.points[2].y - 3, 6, 6, 0xFFFF0000);
		}
	}
	render_color_buffer();
}

void free_resources()
{
	free_mesh();
}

int main(int argc, char* args[])
{
	is_running = initialize_window();

	setup();

	while (is_running)
	{
		process_input();

		update();

		render();
	}

	destroy_window();

	free_resources();

	return 0;
}