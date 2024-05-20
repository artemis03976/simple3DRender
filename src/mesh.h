#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

typedef struct{
    vec3_t *vertices;
    face_t *faces;
    upng_t *texture;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

int getNumMeshes(void);
mesh_t *getMesh(int index);

void load_obj_file(mesh_t *mesh, const char *filename);
void load_png_texture_data(mesh_t *mesh, const char *file_name);

void load_mesh(
    const char *obj_file_name,
    const char *texture_file_name,
    vec3_t scale,
    vec3_t rotation,
    vec3_t translation
);

void free_mesh(void);

#endif //MESH_H
