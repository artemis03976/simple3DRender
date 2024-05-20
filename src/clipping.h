#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"
#include "triangle.h"
#define MAX_POLY_VERTICES 10
#define MAX_POLY_TRIANGLES 8

enum{
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct{
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct{
    vec3_t vertices[MAX_POLY_VERTICES];
    tex2_t texcoords[MAX_POLY_VERTICES];
    int num_vertices;
} polygon_t;

void initialize_frustum_plane(float fov_x, float fov_y, float z_near, float z_far);
polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2);
void clip_polygon(polygon_t* polygon);
void break_polygon(polygon_t* polygon, triangle_t triangles[], int *num_triangles);

#endif //CLIPPING_H
