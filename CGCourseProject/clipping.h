#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"
#include "triangle.h"
#include "texture.h"


#define MAX_NUM_POLYGON_VERTICES 10
#define MAX_NUM_POLYGON_TRIANGLES 10


enum
{
	LEFT_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	TOP_FRUSTUM_PLANE,
	BOTTOM_FRUSTUM_PLANE,
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE
};

typedef struct
{
	vec3_t point;
	vec3_t normal;
}plane_t;

typedef struct
{
	vec3_t vertices[MAX_NUM_POLYGON_VERTICES];
	tex2_t texcoords[MAX_NUM_POLYGON_VERTICES];
	int num_vertices;
	
}polygon_t;

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far);

polygon_t create_polygon_from_triangle(
	vec3_t vertex_a, tex2_t uv_a,
	vec3_t vertex_b, tex2_t uv_b, 
	vec3_t vertex_c, tex2_t uv_c
	);

void create_triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* triangles_num);

void clip_polygon(polygon_t* polygon);

void clip_polygon_against_plane(polygon_t* polygon, int plane_side);

extern plane_t frustum_planes[];

float float_lerp(float a, float b, float t);


#endif // !CLIPPING_H
