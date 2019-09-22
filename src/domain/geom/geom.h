#ifndef DOMAIN_GEOM_H
#define DOMAIN_GEOM_H

#include <stdbool.h>
#include <stddef.h>

#include "../../geom/vertex.h"

static inline void vertex_2f_rgba_copy_triangle(const vertex_2f_rgba src[3],
    vertex_2f_rgba dest[3])
{
    vertex_2f_rgba_copy(&src[0], &dest[0]);
    vertex_2f_rgba_copy(&src[1], &dest[1]);
    vertex_2f_rgba_copy(&src[2], &dest[2]);
}

static inline void vertex_2f_rgba_reverse_triangle(const vertex_2f_rgba src[3],
    vertex_2f_rgba dest[3])
{
    vertex_2f_rgba_copy(&src[0], &dest[2]);
    vertex_2f_rgba_copy(&src[1], &dest[1]);
    vertex_2f_rgba_copy(&src[2], &dest[0]);
}

static inline void vertex_2f_rgba_push_triangle(vertex_2f_rgba* vertices,
    const vertex_2f_rgba triangle[3], bool clockwise)
{
    if (!vertices) {
        return;
    }
    // default winding order is counterclockwise - reverse triangle
    vertex_2f_rgba t[3];
    vertex_2f_rgba_copy_triangle(triangle, t);
    if (clockwise) {
        vertex_2f_rgba_reverse_triangle(t, t);
    }
    vertex_2f_rgba* iter = vertices;
    for (size_t i = 0; i < 3; ++i) {
        vertex_2f_rgba_copy(&t[i], iter);
        ++iter;
    }
}

#endif
