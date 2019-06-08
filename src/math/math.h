#ifndef BASIC_MATH_H
#define BASIC_MATH_H

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
