#ifndef GEOM_VERTEX_H
#define GEOM_VERTEX_H

typedef struct vertex_2f {
    float x, y;
} vertex_2f;

typedef struct vertex_2f_uv {
    float x, y;
    float u, v;
} vertex_2f_uv;

typedef struct vertex_2f_rgba {
    float x, float y;
    float r, g, b, a;
} vertex_2f_rgba;

#endif
