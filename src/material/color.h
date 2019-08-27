#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

static inline void color_rgb_uint32_to_float(float dest[4], uint32_t color) {
    const static float divisor = 0.00392156862; // 1 / 255
    uint32_t r = (color & 0xFF000000) >> 24;
    uint32_t g = (color & 0x00FF0000) >> 16;
    uint32_t b = (color & 0x0000FF00) >> 8;
    uint32_t a = color & 0x000000FF;
    dest[0] = r * divisor; dest[1] = g * divisor;
    dest[2] = b * divisor; dest[3] = a * divisor;
}

#endif
