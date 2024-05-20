#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include <stdint.h>

typedef struct {
    vec3_t direction;
} light_t;

void init_light(vec3_t direction);
vec3_t getLightDirection(void);

uint32_t light_with_intensity(uint32_t origin_color, float intensity_factor);

#endif //LIGHT_H
