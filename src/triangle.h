#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include "texture.h"
#include "upng.h"
#include <stdint.h>

typedef struct {
    int a, b, c;
    tex2_t a_uv, b_uv, c_uv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t tex_coords[3];
    uint32_t color;
    upng_t *texture;
} triangle_t;

vec3_t getTriangleNormal(vec4_t vertices[3]);

void draw_triangle(vec4_t point_0, vec4_t point_1, vec4_t point_2, uint32_t color);
void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
);
void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    upng_t *texture
);
void draw_texel(
    int x, int y, upng_t *texture,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
);

#endif //TRIANGLE_H
