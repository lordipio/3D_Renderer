#include "clipping.h"

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];


float float_lerp(float a, float b, float t)
{
	return a + t * (b - a);	
}

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far)
{
	float sin_half_fovy = sin(fovy / 2);
	float cos_half_fovy = cos(fovy / 2);
	
	float sin_half_fovx = sin(fovx / 2);
	float cos_half_fovx = cos(fovx / 2);

	vec3_t origin = vec3_new(0, 0, 0);
	
	// LEFT PLANE
	frustum_planes[LEFT_FRUSTUM_PLANE].point = origin;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal = vec3_new(cos_half_fovx, 0, sin_half_fovx);

	// RIGHT PLANE
	frustum_planes[RIGHT_FRUSTUM_PLANE].point = origin;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal = vec3_new(-cos_half_fovx,0, sin_half_fovx);

	// TOP PLANE
	frustum_planes[TOP_FRUSTUM_PLANE].point = origin;
	frustum_planes[TOP_FRUSTUM_PLANE].normal = vec3_new(0, -cos_half_fovy, sin_half_fovy);

	// BUTTOM PLANE
	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = origin;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal = vec3_new(0, cos_half_fovy, sin_half_fovy);

	// NEAR PLANE
	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal = vec3_new(0, 0, 1);
	
	// FAR PLANE
	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal = vec3_new(0, 0, -1);
}

polygon_t create_polygon_from_triangle(
	vec3_t vertex_a, tex2_t uv_a,
	vec3_t vertex_b, tex2_t uv_b,
	vec3_t vertex_c, tex2_t uv_c
)
{
	polygon_t polygon = 
	{ 
		.vertices[0] = vertex_a,
		.vertices[1] = vertex_b,
		.vertices[2] = vertex_c,

		.texcoords[0] = uv_a,
		.texcoords[1] = uv_b,
		.texcoords[2] = uv_c,

		.num_vertices = 3 
	};
	return polygon;
}

void create_triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* triangles_num)
{
	for (int i = 0; i < polygon->num_vertices - 2; i++)
	{
		int q1 = 0;
		int q2 = i + 1;
		int q3 = i + 2;

		triangles[i].points[0] = vec3_to_vec4(polygon->vertices[q1]);
		triangles[i].points[1] = vec3_to_vec4(polygon->vertices[q2]);
		triangles[i].points[2] = vec3_to_vec4(polygon->vertices[q3]);
		
		triangles[i].uv_coordinate[0] = polygon->texcoords[q1];
		triangles[i].uv_coordinate[1] = polygon->texcoords[q2];
		triangles[i].uv_coordinate[2] = polygon->texcoords[q3];
	}

	*triangles_num = polygon->num_vertices - 2;

}

void clip_polygon(polygon_t* polygon)
{
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane_side)
{
	vec3_t plane_normal = frustum_planes[plane_side].normal;
	vec3_t plane_point = frustum_planes[plane_side].point;

	int num_inside_vertices = 0;

	vec3_t* current_vertex = &polygon->vertices[0];
	vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];

	tex2_t* current_texcoord = &polygon->texcoords[0];
	tex2_t* previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

	vec3_t inside_vertices[MAX_NUM_POLYGON_VERTICES];
	tex2_t inside_texcoords[MAX_NUM_POLYGON_VERTICES];

	float current_dot;
	float previous_dot = vec3_dot(plane_normal, (vec3_sub(*previous_vertex, plane_point)));

	//while (current_index != num_vertices - 1)
	while (current_vertex != &polygon->vertices[polygon->num_vertices])
	{
		current_dot = vec3_dot(plane_normal, vec3_sub(*current_vertex, plane_point));


		if (current_dot * previous_dot < 0)
		{
			// calculate I current_vertex
			float t = previous_dot / (previous_dot - current_dot);
			
			vec3_t intersected_point;
			tex2_t intersected_texcoord;

			
			intersected_point.x = float_lerp(previous_vertex->x, current_vertex->x, t);
			intersected_point.y = float_lerp(previous_vertex->y, current_vertex->y, t);
			intersected_point.z = float_lerp(previous_vertex->z, current_vertex->z, t);
			
			intersected_texcoord.u = float_lerp(previous_texcoord->u, current_texcoord->u, t);
			intersected_texcoord.v = float_lerp(previous_texcoord->v, current_texcoord->v, t);

			inside_vertices[num_inside_vertices] = intersected_point;
			inside_texcoords[num_inside_vertices] = intersected_texcoord;

			num_inside_vertices++;
			
		}

		if (current_dot > 0)
		{
			inside_vertices[num_inside_vertices] = *current_vertex;
			inside_texcoords[num_inside_vertices] = *current_texcoord;
			num_inside_vertices++;
		}

		previous_vertex = current_vertex;
		previous_texcoord = current_texcoord;

		previous_dot = current_dot;

		current_vertex++;
		current_texcoord++;
	}

	
	for (int i = 0; i < num_inside_vertices; i++) {
		polygon->vertices[i] = inside_vertices[i];
		polygon->texcoords[i] = inside_texcoords[i];
	}

	polygon->num_vertices = num_inside_vertices;
}
