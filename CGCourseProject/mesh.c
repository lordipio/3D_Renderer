#include "mesh.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>

//static mesh_t mesh;
#define MAX_NUM_MESHES 10
static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

mesh_t mesh = { 
	.vertices = NULL,
	.faces = NULL,
	.rotation = {0, 0, 0},
	.scale = {1.f, 1.f, 1.f},
	.translation = {0.f, 0.f, 0.f}
};

////////////////////////////////////
// HARDCODED CUBE
////////////////////////////////////

vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{.x = -1, .y = -1, .z = -1}, //1
	{.x = -1, .y =  1, .z = -1}, //2
	{.x =  1, .y =  1, .z = -1}, //3
	{.x =  1, .y = -1, .z = -1}, //4
	{.x =  1, .y =  1, .z =  1}, //5
	{.x =  1, .y = -1, .z =  1}, //6
	{.x = -1, .y =  1, .z =  1}, //7
	{.x = -1, .y = -1, .z =  1}, //8
};

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	// right
	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	// back
	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	// left
	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	// top
	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	// bottom
	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF }
};


void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
        array_push(mesh.vertices, cube_vertices[i]);

    for (int i = 0; i < N_CUBE_FACES; i++)
        array_push(mesh.faces, cube_faces[i]);
}

int get_num_meshes(void)
{
    return mesh_count;
}

mesh_t* get_mesh(int index)
{
    return &meshes[index];
}

void free_mesh()
{
    for (int i = 0; i < mesh_count; i++)
    {
        array_free(meshes[i].faces);
        array_free(meshes[i].vertices);
        upng_free(meshes[i].texture);
    }
}

////////////////////////////////////

void load_obj_file_data(mesh_t* mesh, char* filename) {
    tex2_t* uv_coordinates = NULL;

    FILE* file;
    fopen_s(&file, filename, "r");
    char line[1024];

    tex2_t* texcoords = NULL;

    if (file == NULL)
    {
        printf("can't open up the file!");
        return;
    }

    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh->vertices, vertex);
        }
        // Texture coordinate information
        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }
        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf_s(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );
            face_t face = {
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texcoords[texture_indices[0] - 1],
                .b_uv = texcoords[texture_indices[1] - 1],
                .c_uv = texcoords[texture_indices[2] - 1],
                .color = 0xFFFFFFFF
            };
            array_push(mesh->faces, face);
        }
    }

    array_free(texcoords);
}

void load_mesh(char* filename, char* png_filename, vec3_t rotation, vec3_t scale, vec3_t translation)
{
    load_obj_file_data(&meshes[mesh_count], filename);
    load_mesh_png_texture_data(&meshes[mesh_count], png_filename);

    meshes[mesh_count].scale = scale;
    meshes[mesh_count].rotation = rotation;
    meshes[mesh_count].translation = translation;

    mesh_count++;
}


void load_mesh_png_texture_data(mesh_t* mesh, char* filename)
{
    upng_t* png_image = upng_new_from_file(filename);
    if (png_image != NULL)
    {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK)
        {
            mesh->texture = png_image;
        }
    }
}