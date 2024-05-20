#include "triangle.h"
#include "display.h"
#include "swap.h"

vec3_t getTriangleNormal(vec4_t vertices[3]){
    vec3_t vector_a = vec3_from_vec4(vertices[0]);
    vec3_t vector_b = vec3_from_vec4(vertices[1]);
    vec3_t vector_c = vec3_from_vec4(vertices[2]);

    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
    vec3_t vector_ac = vec3_sub(vector_c, vector_a);
    vec3_normalize(&vector_ab);
    vec3_normalize(&vector_ac);

    vec3_t normal = vec3_cross(vector_ab, vector_ac);
    vec3_normalize(&normal);

    return normal;
}

void draw_triangle(vec4_t point_0, vec4_t point_1, vec4_t point_2, uint32_t color){
    draw_line(point_0.x, point_0.y, point_1.x, point_1.y, color);
    draw_line(point_1.x, point_1.y, point_2.x, point_2.y, color);
    draw_line(point_2.x, point_2.y, point_0.x, point_0.y, color);
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p){
    vec2_t ab = vec2_sub(b, a);
    vec2_t ac = vec2_sub(c, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    vec2_t ap = vec2_sub(p, a);

    float area_abc = vec2_cross(ac, ab);
    float area_pbc = vec2_cross(pc, pb);
    float area_apc = vec2_cross(ac, ap);

    float alpha = area_pbc / area_abc;
    float beta = area_apc / area_abc;
    float gamma = 1 - alpha - beta;

    vec3_t weights = {alpha, beta, gamma};
    return weights;
}

void draw_triangle_pixel(int x, int y, uint32_t color, vec4_t point_a, vec4_t point_b, vec4_t point_c){
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weight = barycentric_weights(a, b, c, p);
    float alpha = weight.x;
    float beta = weight.y;
    float gamma = weight.z;

    float inv_w = (1.0 / point_a.w) * alpha + (1.0 / point_b.w) * beta + (1.0 / point_c.w )* gamma;

    // override inv_w so that smaller values mean closer to screen
    inv_w = 1.0 - inv_w;

    // only draw pixel if the depth is less than the z buffer
    if (inv_w < getZBufferAt(x, y)){
        draw_pixel(x, y, color);
        // update z buffer
        setZBufferAt(x, y, inv_w);
    }
}

void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
){
    // sort vertices by y
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }

    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    // draw top of the triangle
    if (y1 != y0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 != y0){
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) int_swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++){
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }

    // draw bottom of the triangle
    if (y2 != y1) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 != y1){
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) int_swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }
}

void draw_texel(
    int x, int y, upng_t *texture,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
){
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weight = barycentric_weights(a, b, c, p);
    float alpha = weight.x;
    float beta = weight.y;
    float gamma = weight.z;

    float u;
    float v;
    float inv_w;

    u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
    inv_w = (1.0 / point_a.w) * alpha + (1.0 / point_b.w) * beta + (1.0 / point_c.w )* gamma;

    u /= inv_w;
    v /= inv_w;

    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);

    int texel_x = abs((int)(u * texture_width)) % texture_width;
    int texel_y = abs((int)(v * texture_height)) % texture_height;



    // override inv_w so that smaller values mean closer to screen
    inv_w = 1.0 - inv_w;

    // only draw pixel if the depth is less than the z buffer
    if (inv_w < getZBufferAt(x, y)){
        // get buffer from the pointer
        uint32_t *texture_buffer = (uint32_t*) upng_get_buffer(texture);
        uint32_t texel = texture_buffer[texel_y * texture_width + texel_x];
        draw_pixel(x, y, texel);
        // update z buffer
        setZBufferAt(x, y, inv_w);
    }
}

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    upng_t *texture
){
    // sort vertices by y
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }

    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };
    tex2_t a_uv = { u0, v0 };
    tex2_t b_uv = { u1, v1 };
    tex2_t c_uv = { u2, v2 };

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    // draw top of the triangle
    if (y1 != y0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 != y0){
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) int_swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++){
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    // draw bottom of the triangle
    if (y2 != y1) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 != y0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 != y1){
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) int_swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}