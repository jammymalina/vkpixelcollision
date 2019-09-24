#include "./geom_generator.h"

#include "../../logger/logger.h"

void geom_shape_generate(vertex_2f_rgba* vertices, size_t* vertex_count, const
    geom_shape_generate_info* shape_params)
{
    switch (shape_params->shape) {
        case GEOM_SHAPE_CIRCLE:
            circle_generate(vertices, vertex_count,
                &shape_params->circle_config);
            return;
        default:
            log_warning("Unable to generate shape: unknown shape");
            return;
    }
}
