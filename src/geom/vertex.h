#ifndef GEOM_VERTEX_H
#define GEOM_VERTEX_H

typedef struct vertex_2f {
    float position[2];
} vertex_2f;

typedef struct vertex_2f_uv {
    float position[2];
    float uv[2];
} vertex_2f_uv;

typedef struct vertex_2f_rgba {
    float position[2];
    float rgba[4];
} vertex_2f_rgba;

static inline void vertex_2f_rgba_copy(const vertex_2f_rgba* src, vertex_2f_rgba*
    dest)
{
    dest->position[0] = src->position[0]; dest->position[1] = src->position[1];
    dest->rgba[0] = src->rgba[0]; dest->rgba[1] = src->rgba[1];
    dest->rgba[2] = src->rgba[2]; dest->rgba[3] = src->rgba[3];
}

#endif
