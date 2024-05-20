#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw_angle;
    float pitch_angle;
} camera_t;

void init_camera(vec3_t position, vec3_t direction);

vec3_t getCameraPosition(void);
vec3_t getCameraDirection(void);
vec3_t getCameraForwardVelocity(void);
float getCameraYawAngle(void);
float getCameraPitchAngle(void);

void setCameraPosition(vec3_t position);
void setCameraDirection(vec3_t direction);
void setCameraForwardVelocity(vec3_t forward_velocity);
void setCameraYawAngle(float yaw_angle);
void setCameraPitchAngle(float pitch_angle);

void moveCamera(char dir, float distance);
vec3_t getLookAtTarget(void);

#endif //CAMERA_H
