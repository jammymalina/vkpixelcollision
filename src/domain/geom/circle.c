#include "./circle.h"

#include "../../math/math.h"
#include "../../material/color.h"

void circle_generate(vertex_2f_rgba* vertices, size_t* vertex_count, const
    circle_params* params)
{
    const float radius = params->radius == 0 ? 1.0 : params->radius;
    const size_t segments = max(3, params->segments);

    const uint32_t v_count = segments + 2;

    float rgba[4] = { 0, 0, 0, 0 };
    color_rgba_uint32_to_float(rgba, params->fill_style);

    vertex_2f_rgba center = {
        .position = { params->cx, params->cy },
        .rgba = { rgba[0], rgba[1], rgba[2], rgba[3] }
    };

    vertex_2f_rgba first_vertex = { 0 };
    vertex_2f_rgba prev_vertex = { 0 };
    vertex_2f_rgba* v_iter = vertices;

    v_iter++;

    for (uint32_t s = 0; s < v_count - 1; ++s) {
        if (vertices) {
            float segment = params->theta_start + (((float) s) / segments) *
                params->theta_length;

            float x = radius * cos(segment);
            float y = radius * sin(segment);

            vertex_2f_rgba v = {
                .position = { x, y },
                .rgba = { rgba[0], rgba[1], rgba[2], rgba[3] }
            };

            if (s == v_count - 2 && params->theta_length >= PI_F_2 - BIAS_F) {
                vertex_2f_rgba_copy(&first_vertex, &v);
            }

            if (s == 0) {
                vertex_2f_rgba_copy(&v, &first_vertex);
            } else {
                vertex_2f_rgba triangle[3] = {
                    center, v, prev_vertex
                };
                vertex_2f_rgba_push_triangle(vertices, triangle,
                    params->clockwise);
            }
            vertex_2f_rgba_copy(&v, &prev_vertex);
            v_iter++;
        }
    }

    if (vertex_count) {
        *vertex_count = 3 * segments;
    }
}
