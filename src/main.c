#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0.0;

mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

void setup(void){
    // initialize scene light
    init_light(vec3_new(0, 0, 1));
    // initialize camera
    init_camera(vec3_new(0, 0, 0), vec3_new(0, 0, 10));

    // initialize the perspective matrix
    float aspect_y = (float)getWindowHeight() / (float)getWindowWidth();
    float aspect_x = (float)getWindowWidth() / (float)getWindowHeight();
    float fov_y = M_PI / 3.0;
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2.0;
    float z_near = 0.5;
    float z_far = 60.0;
    proj_matrix = mat4_perspective(fov_y, aspect_y, z_near, z_far);

    // initialize frustum planes
    initialize_frustum_plane(fov_x, fov_y, z_near, z_far);

    // load meshes and textures
    load_mesh(
        "../assets/f117.obj",
        "../assets/f117.png",
        vec3_new(1, 1, 1),
        vec3_new(0, -M_PI / 2, 0),
        vec3_new(-2, -1.3, 9)
    );
    load_mesh(
        "../assets/f22.obj",
        "../assets/f22.png",
        vec3_new(1, 1, 1),
        vec3_new(0, -M_PI / 2, 0),
        vec3_new(2, -1.3, 9)
    );
    load_mesh(
            "../assets/efa.obj",
            "../assets/efa.png",
            vec3_new(1, 1, 1),
            vec3_new(0, -M_PI / 2, 0),
            vec3_new(0, -1.3, 5)
    );
    load_mesh(
            "../assets/runway.obj",
            "../assets/runway.png",
            vec3_new(1, 1, 1),
            vec3_new(0, 0, 0),
            vec3_new(0, -1.5, 23)
    );
}

void process_input(void){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }
                if (event.key.keysym.sym == SDLK_1){
                    RenderMode_Vertex = !RenderMode_Vertex;
                    break;
                }
                if (event.key.keysym.sym == SDLK_2){
                    RenderMode_Wireframe = !RenderMode_Wireframe;
                    break;
                }
                if (event.key.keysym.sym == SDLK_3){
                    RenderMode_Fill = !RenderMode_Fill;
                    break;
                }
                if (event.key.keysym.sym == SDLK_4){
                    RenderMode_Texture = !RenderMode_Texture;
                    break;
                }
                if (event.key.keysym.sym == SDLK_RETURN){
                    CullMode_Back = !CullMode_Back;
                    break;
                }
                if (event.key.keysym.sym == SDLK_w){
                    setCameraForwardVelocity(vec3_mul(getCameraDirection(), 5.0 * delta_time));
                    setCameraPosition(vec3_add(getCameraPosition(), getCameraForwardVelocity()));
                    break;
                }
                if (event.key.keysym.sym == SDLK_s){
                    setCameraForwardVelocity(vec3_mul(getCameraDirection(), 5.0 * delta_time));
                    setCameraPosition(vec3_sub(getCameraPosition(), getCameraForwardVelocity()));
                    break;
                }
                if (event.key.keysym.sym == SDLK_a){
                    moveCamera('x', -3.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_d){
                    moveCamera('x', 3.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_SPACE){
                    moveCamera('y', 3.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_LSHIFT){
                    moveCamera('y', -3.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_UP){
                    setCameraPitchAngle(-1.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_DOWN){
                    setCameraPitchAngle(1.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_LEFT){
                    setCameraYawAngle(-1.0 * delta_time);
                    break;
                }
                if (event.key.keysym.sym == SDLK_RIGHT){
                    setCameraYawAngle(1.0 * delta_time);
                    break;
                }
                break;
        }
    }
}

void process_graphic_pipeline_stages(mesh_t *mesh){
    // create transformation matrix
    mat4_t scale_matrix = mat4_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_rotation_z(mesh->rotation.z);

    // initialize target
    vec3_t target = getLookAtTarget();
    vec3_t up = vec3_new(0, 1, 0);
    // create view matrix
    view_matrix = mat4_look_at(getCameraPosition(), target, up);

    int num_faces = array_size(mesh->faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh->faces[i];

        // set face vertices
        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a - 1];
        face_vertices[1] = mesh->vertices[mesh_face.b - 1];
        face_vertices[2] = mesh->vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // Loop through each vertex in the face and apply transformation
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // init world matrix
            world_matrix = mat4_identity();
            // multiply scale matrix
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            // multiply rotation matrix
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            // multiply translation matrix
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
            // multiply world matrix
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // multiply view matrix
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // apply backface culling
        vec3_t face_normal = getTriangleNormal(transformed_vertices);

        if (CullMode_Back) {
            vec3_t camera_ray = vec3_sub(vec3_new(0, 0, 0), vec3_from_vec4(transformed_vertices[0]));
            if (vec3_dot(face_normal, camera_ray) < 0) {
                continue;
            }
        }

        // create polygon for clipping
        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );

        // apply clipping
        clip_polygon(&polygon);
        // break polygon into triangle
        triangle_t triangles_clipped[MAX_POLY_TRIANGLES];
        int num_triangles_clipped = 0;
        break_polygon(&polygon, triangles_clipped, &num_triangles_clipped);

        // apply projection for clipped triangles
        for (int k = 0; k < num_triangles_clipped; k++) {
            triangle_t triangle = triangles_clipped[k];

            vec4_t projected_points[3];

            // Loop through each vertex in the face and project it to 2d
            for (int j = 0; j < 3; j++) {
                // project point to 2d
                projected_points[j] = mat4_project(proj_matrix, triangle.points[j]);

                // scale point to fit the screen
                projected_points[j].x *= (getWindowWidth() / 2.0);
                projected_points[j].y *= -(getWindowHeight() / 2.0);
                // translate point to center of the screen
                projected_points[j].x += (getWindowWidth() / 2.0);
                projected_points[j].y += (getWindowHeight() / 2.0);
            }

            // calculate shade intensity
            float light_intensity_factor = -vec3_dot(face_normal, getLightDirection());
            // calculate color based on the light
            uint32_t triangle_color = light_with_intensity(mesh_face.color, light_intensity_factor);

            // set triangle to be rendered
            triangle_t render_triangle = {
                    .points = {
                            {projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                            {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                            {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
                    },
                    .tex_coords = {
                            {triangle.tex_coords[0].u, triangle.tex_coords[0].v},
                            {triangle.tex_coords[1].u, triangle.tex_coords[1].v},
                            {triangle.tex_coords[2].u, triangle.tex_coords[2].v}
                    },
                    .color = triangle_color,
                    .texture = mesh->texture
            };

            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render++] = render_triangle;
            }
        }
    }
}

void update(void){
    // fix frame rate
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // time factor
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    // update previous frame time
    previous_frame_time = SDL_GetTicks();

    num_triangles_to_render = 0;

    for (int mesh_idx = 0; mesh_idx < getNumMeshes(); mesh_idx++) {
        mesh_t *mesh = getMesh(mesh_idx);
        // change values per frame
        //    mesh.rotation.x += 0.6 * delta_time;
        //    mesh.rotation.y += 0.6 * delta_time;
        //    mesh.rotation.z += 0.6 * delta_time;

        //    mesh.scale.x += 0.001 * delta_time;
        //    mesh.scale.y += 0.001 * delta_time;
        //    mesh.scale.z += 0.001 * delta_time;
        //
        //    mesh.translation.x += 0.01 * delta_time;
        //    mesh.translation.y += 0.01 * delta_time;
        //    mesh.translation.z = 5.0;

        // process mesh
        process_graphic_pipeline_stages(mesh);
    }
}

void render(void){
    // clear buffer before new render
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    draw_grid();

    // apply different render mode
    for (int i = 0; i < num_triangles_to_render; i++){
        triangle_t triangle = triangles_to_render[i];

        if (RenderMode_Fill){
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }
        if (RenderMode_Wireframe){
            draw_triangle(
                triangle.points[0],
                triangle.points[1],
                triangle.points[2],
                0xFFFFFFFF
            );
        }
        if (RenderMode_Vertex){
            draw_rect(triangle.points[0].x, triangle.points[0].y, 6, 6, 0xFFFFFF00);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 6, 6, 0xFFFFFF00);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 6, 6, 0xFFFFFF00);
        }
        if (RenderMode_Texture){
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.tex_coords[0].u, triangle.tex_coords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.tex_coords[1].u, triangle.tex_coords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.tex_coords[2].u, triangle.tex_coords[2].v,
                triangle.texture
            );
        }
    }
    render_color_buffer();
}

void free_resources(void){
    free_mesh();
    destroy_window();
}

int main(int argc, char *argv[]) {
    is_running = initialize_window();

    setup();

    while(is_running){
        process_input();
        update();
        render();
    }

    free_resources();

    return 0;
}
