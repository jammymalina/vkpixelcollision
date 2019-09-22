#ifndef BASIC_MATH_H
#define BASIC_MATH_H

#include <math.h>

static const double PI = 3.14159265359;
#define PI_2 (2.0 * PI)
#define PI_HALF (0.5 * PI)

static const float PI_F = 3.14159265359;
#define PI_F_2 (2.0 * PI_F)
#define PI_F_HALF (0.5 * PI_F)

static const double BIAS = 0.00005;
static const float BIAS_F = 0.00005;

#define max(a, b)                                            \
    ({                                                       \
        __typeof__(a) _a = (a);                              \
        __typeof__(b) _b = (b);                              \
        _a > _b ? _a : _b;                                   \
    })

#define min(a, b)                                            \
    ({                                                       \
        __typeof__(a) _a = (a);                              \
        __typeof__(b) _b = (b);                              \
       _a < _b ? _a : _b;                                    \
    })

#define clamp(x, low, high)                                  \
    ({                                                       \
        __typeof__(x) __x = (x);                             \
        __typeof__(low) __low = (low);                       \
        __typeof__(high) __high = (high);                    \
        __x > __high ? __high : (__x < __low ? __low : __x); \
    })

#endif
