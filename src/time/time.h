#ifndef CUSTOM_TIME_UTIL_H
#define CUSTOM_TIME_UTIL_H

#include <SDL2/SDL.h>
#include <stdint.h>

static inline uint32_t time_relative_timestamp_ms() {
    return SDL_GetTicks();
}

#endif
