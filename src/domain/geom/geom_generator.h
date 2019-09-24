#ifndef DOMAIN_GEOM_GENERATOR_H
#define DOMAIN_GEOM_GENERATOR_H

#include "./circle.h"

typedef enum geom_shape {
    GEOM_SHAPE_CIRCLE,
    GEOM_SHAPE_TOTAL
} geom_shape;

typedef struct geom_shape_generate_info {
    geom_shape shape;
    union {
        circle_params circle_config;
    };
} geom_shape_generate_info;

void geom_shape_generate(vertex_2f_rgba* vertices, size_t* vertex_count, const
    geom_shape_generate_info* shape_params);

#endif
