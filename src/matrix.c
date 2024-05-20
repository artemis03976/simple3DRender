#include "matrix.h"
#include <math.h>

mat4_t mat4_identity(void){
    mat4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz){
    mat4_t m = mat4_identity();

    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

mat4_t mat4_translation(float tx, float ty, float tz){
    mat4_t m = mat4_identity();

    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;

    return m;
}

mat4_t mat4_rotation_x(float angle){
    mat4_t m = mat4_identity();

    float sin_a = sin(angle);
    float cos_a = cos(angle);

    m.m[1][1] = cos_a;
    m.m[1][2] = -sin_a;
    m.m[2][1] = sin_a;
    m.m[2][2] = cos_a;

    return m;
}

mat4_t mat4_rotation_y(float angle){
    mat4_t m = mat4_identity();

    float sin_a = sin(angle);
    float cos_a = cos(angle);

    m.m[0][0] = cos_a;
    m.m[0][2] = sin_a;
    m.m[2][0] = -sin_a;
    m.m[2][2] = cos_a;

    return m;
}

mat4_t mat4_rotation_z(float angle){
    mat4_t m = mat4_identity();

    float sin_a = sin(angle);
    float cos_a = cos(angle);

    m.m[0][0] = cos_a;
    m.m[0][1] = -sin_a;
    m.m[1][0] = sin_a;
    m.m[1][1] = cos_a;

    return m;
}

mat4_t mat4_perspective(float fov, float aspect, float z_near, float z_far){
    mat4_t m = {{{0}}};

    float tan_half_fov = tan(fov / 2);

    m.m[0][0] = aspect / tan_half_fov;
    m.m[1][1] = 1 / (tan_half_fov);
    m.m[2][2] = z_far / (z_far - z_near);
    m.m[2][3] = -(z_far * z_near) / (z_far - z_near);
    m.m[3][2] = 1.0;

    return m;
}

vec4_t mat4_project(mat4_t proj_mat, vec4_t v){
    vec4_t result = mat4_mul_vec4(proj_mat, v);

    // do perspective divide
    if (result.w != 0.0){
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up){
    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);

    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x);

    vec3_t y = vec3_cross(z, x);

    mat4_t view_matrix = {{
      {x.x, x.y, x.z, -vec3_dot(x, eye)},
      {y.x, y.y, y.z, -vec3_dot(y, eye)},
      {z.x, z.y, z.z, -vec3_dot(z, eye)},
      {0, 0, 0, 1}
    }};

    return view_matrix;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v){
    vec4_t result;

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

    return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b){
    mat4_t result;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }

    return result;
}