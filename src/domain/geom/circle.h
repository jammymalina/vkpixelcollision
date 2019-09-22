#ifndef GEOM_POLYGON_H
#define GEOM_POLYGON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../geom/vertex.h"
#include "./geom.h"

typedef struct circle_params {
    uint32_t fill_style;
    float cx, cy;
    float radius;
    size_t segments;
    float theta_start, theta_length;
    bool clockwise;
} circle_params;

void circle_generate(vertex_2f_rgba* vertices, size_t* vertex_count, const
    circle_params* params);

#endif
