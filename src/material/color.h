#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

static inline void color_rgba_uint32_to_uint(uint32_t dest[4], uint32_t color) {
    dest[0] = (color & 0xff000000) >> 24;
    dest[1] = (color & 0x00ff0000) >> 16;
    dest[2] = (color & 0x0000ff00) >> 8;
    dest[3] = (color & 0x000000ff);
}

static inline void color_rgba_uint32_to_float(float dest[4], uint32_t color) {
    const static float divisor = 0.00392156862; // 1 / 255
    uint32_t rgba[4] = { 0, 0, 0, 0 };
    color_rgba_uint32_to_uint(rgba, color);
    dest[0] = rgba[0] * divisor; dest[1] = rgba[1] * divisor;
    dest[2] = rgba[2] * divisor; dest[3] = rgba[3] * divisor;
}

#endif
