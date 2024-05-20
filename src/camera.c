#include "camera.h"
#include "matrix.h"

static camera_t camera;

void init_camera(vec3_t position, vec3_t direction){
    camera.position = position;
    camera.direction = direction;
    camera.forward_velocity = vec3_new(0.0, 0.0, 0.0);
    camera.yaw_angle = 0.0;
    camera.pitch_angle = 0.0;
}

vec3_t getCameraPosition(void){
    return camera.position;
}

vec3_t getCameraDirection(void){
    return camera.direction;
}

vec3_t getCameraForwardVelocity(void){
    return camera.forward_velocity;
}

float getCameraYawAngle(void){
    return camera.yaw_angle;
}

float getCameraPitchAngle(void){
    return camera.pitch_angle;
}

void setCameraPosition(vec3_t position){
    camera.position = position;
}

void setCameraDirection(vec3_t direction){
    camera.direction = direction;
}

void setCameraForwardVelocity(vec3_t forward_velocity){
    camera.forward_velocity = forward_velocity;
}

void setCameraYawAngle(float yaw_angle){
    camera.yaw_angle += yaw_angle;
}

void setCameraPitchAngle(float pitch_angle){
    camera.pitch_angle += pitch_angle;
}

void moveCamera(char dir, float distance){
    if (dir == 'y'){
        camera.position.y += distance;
    }
    else if (dir == 'x'){
        camera.position.x += distance;
    }
    else if (dir == 'z'){
        camera.position.z += distance;
    }
}

vec3_t getLookAtTarget(void){
    vec3_t target = {0, 0, 1};

    mat4_t camera_yaw_rotation = mat4_rotation_y(camera.yaw_angle);
    mat4_t camera_pitch_rotation = mat4_rotation_x(camera.pitch_angle);

    mat4_t camera_rotation = mat4_identity();
    camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(camera_direction);

    // find the target
    target = vec3_add(camera.position, camera.direction);
    return target;
}