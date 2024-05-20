#include <stdio.h>
#include "mesh.h"
#include "array.h"
#include "texture.h"

#define MAX_MESHES 10

static mesh_t meshes[MAX_MESHES];
static int num_meshes = 0;

int getNumMeshes(void){
    return num_meshes;
}

mesh_t *getMesh(int index){
    if(index < 0 || index >= num_meshes) return NULL;
    return &meshes[index];
}

void load_obj_file(mesh_t *mesh, const char *filename) {
    FILE *file;
    char line[1024];
    tex2_t *texcoords = NULL;

    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            float x, y, z;
            float u, v;
            if (line[1] == ' ') {
                sscanf(line, "v %f %f %f", &x, &y, &z);
                vec3_t mesh_vertex = {x, y, z};
                array_push(mesh->vertices, mesh_vertex);
            }
            else if (line[1] == 't') {
                sscanf(line, "vt %f %f", &u, &v);
                tex2_t texcoord = {u, v};
                array_push(texcoords, texcoord);
            }
        }
        else if (line[0] == 'f') {
            int v1, v2, v3;
            int vt1, vt2, vt3;
            int vn1, vn2, vn3;
            if (line[1] == ' ') {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &v1, &vt1, &vn1,
                       &v2, &vt2, &vn2,
                       &v3, &vt3, &vn3
                );
                face_t mesh_face = {
                    .a = v1,
                    .b = v2,
                    .c = v3,
                    .a_uv = texcoords[vt1 - 1],
                    .b_uv = texcoords[vt2 - 1],
                    .c_uv = texcoords[vt3 - 1],
                    .color = 0xFFFFFFFF
                };
                array_push(mesh->faces, mesh_face);
            }
        }
    }
    array_free(texcoords);
    fclose(file);
}

void load_png_texture_data(mesh_t *mesh, const char *file_name){
    upng_t *texture_data = upng_new_from_file(file_name);
    if (texture_data != NULL){
        upng_decode(texture_data);
        if (upng_get_error(texture_data) == UPNG_EOK){
            mesh->texture = texture_data;
        }
    }
}

void load_mesh(
        const char *obj_file_name,
        const char *texture_file_name,
        vec3_t scale,
        vec3_t rotation,
        vec3_t translation
){
    load_obj_file(&meshes[num_meshes], obj_file_name);
    load_png_texture_data(&meshes[num_meshes], texture_file_name);
    meshes[num_meshes].scale = scale;
    meshes[num_meshes].rotation = rotation;
    meshes[num_meshes].translation = translation;

    num_meshes++;
}

void free_mesh(void){
    for (int i = 0; i < num_meshes; i++){
        upng_free(meshes[i].texture);
        array_free(meshes[i].vertices);
        array_free(meshes[i].faces);
    }
}
