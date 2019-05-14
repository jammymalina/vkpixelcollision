#include "./memory.h"

#include <SDL2/SDL.h>

void *mem_alloc(size_t size) {
    return SDL_malloc(size);
}

void mem_free(void *data) {
    SDL_free(data);
}

void mem_copy(void *dest, const void *src, size_t length) {
    SDL_memcpy(dest, src, length);
}
