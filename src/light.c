#include "light.h"

static light_t light;

void init_light(vec3_t direction){
    light.direction = direction;
}

vec3_t getLightDirection(void){
    return light.direction;
}

uint32_t light_with_intensity(uint32_t origin_color, float intensity_factor){
    if (intensity_factor > 1) intensity_factor = 1;
    if (intensity_factor < 0) intensity_factor = 0;

    uint32_t a = (origin_color & 0xFF000000);
    uint32_t r = (origin_color & 0x00FF0000) * intensity_factor;
    uint32_t g = (origin_color & 0x0000FF00) * intensity_factor;
    uint32_t b = (origin_color & 0x000000FF) * intensity_factor;

    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return new_color;
}