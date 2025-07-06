#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

#define N_CUBE_VERTICES 8
extern vec3_t cube_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES 2 * 6
extern face_t cube_faces[N_CUBE_FACES];

typedef struct
{
	vec3_t* vertices;
	upng_t* texture;
	face_t* faces;
	vec3_t rotation;
	vec3_t scale;
	vec3_t translation;
}mesh_t;

void load_mesh(char* filename, char* png_filename, vec3_t rotation, vec3_t scale, vec3_t translation);

void load_mesh_png_texture_data(mesh_t* mesh, char* filename);

void load_cube_mesh_data(void);

int get_num_meshes(void);

mesh_t* get_mesh(int index);

void free_mesh();

extern uint32_t* mesh_texture;

#endif // !MESH_H
