#ifndef MEMORY_HEAP_UTILS_H
#define MEMORY_HEAP_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../logger/logger.h"

#define CHECK_ALLOC(x, s)                          \
    do {                                           \
        if ((x) == NULL) {                         \
            SDL_OutOfMemory();                     \
            log_error("%s: %s", s, #x " == NULL"); \
            exit(EXIT_FAILURE);                    \
        }                                          \
    } while (0)

#define CHECK_ALLOC_BOOL(x, s)                     \
    do {                                           \
        if ((x) == NULL) {                         \
            SDL_OutOfMemory();                     \
            log_error("%s: %s", s, #x " == NULL"); \
            return false;                          \
        }                                          \
    } while (0)

#define COUNT_ONE_BITS(n) ({                       \
    __typeof__ (n) _count = 0;                     \
    __typeof__ (n) _n = (n);                       \
    __typeof__ (n) _one = 1;                       \
    while (_n) {                                   \
        _count += _n & _one;                       \
        _n >>= _one;                               \
    }                                              \
    _count;                                        \
})

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define ALIGN_MEM(x, a) ((((uintptr_t) (x)) + ((a) - 1) ) & ~(((uintptr_t) (a)) - 1))

#define is_2_byte_aligned(ptr)   ((((uintptr_t) (ptr)) &   1) == 0)
#define is_4_byte_aligned(ptr)   ((((uintptr_t) (ptr)) &   3) == 0)
#define is_8_byte_aligned(ptr)   ((((uintptr_t) (ptr)) &   7) == 0)
#define is_16_byte_aligned(ptr)  ((((uintptr_t) (ptr)) &  15) == 0)
#define is_32_byte_aligned(ptr)  ((((uintptr_t) (ptr)) &  31) == 0)
#define is_64_byte_aligned(ptr)  ((((uintptr_t) (ptr)) &  63) == 0)
#define is_128_byte_aligned(ptr) ((((uintptr_t) (ptr)) & 127) == 0)

void* mem_alloc(size_t size);
void* mem_realloc(void* mem, size_t size);
void mem_free(void* data);
void mem_copy(void* dest, const void* src, size_t length);
void* mem_move(void* dest, const void* src, size_t length);
int mem_cmp(const void* m1, const void* m2, size_t length);
void mem_set(void* m1, int value, size_t length);

#endif
