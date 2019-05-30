#ifndef VMA_VECTOR_H
#define VMA_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../../memory/memory.h"

#define VMA_VECTOR(type) {                                                                                   \
    size_t cap;                                                                                              \
    size_t size;                                                                                             \
    type* data;                                                                                              \
}

#define VMA_VECTOR_INITIALIZER { 0, 0, NULL }

#define vma_vector_init(pv) (void) (                                                                         \
    (pv)->cap = 0,                                                                                           \
    (pv)->size = 0,                                                                                          \
    (pv)->data = NULL                                                                                        \
)

#define vma_vector_destroy(pv) free((pv)->data)

#define vma_vector_clear(pv) (                                                                               \
    vma_vector_destroy(pv),                                                                                  \
    vma_vector_init(pv)                                                                                      \
)

#define VMA_VECTOR_MINCAP_ ((size_t) 10)

static inline size_t vma_vector_min_(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline size_t vma_vector_max_(size_t a, size_t b) {
    return a > b ? a : b;
}

static inline size_t vma_vector_between_(size_t x, size_t min, size_t max) {
    return vma_vector_max_(min, vma_vector_min_(max, x));
}

static inline size_t vma_vector_enforce_size_t_(size_t value) {
    return value;
}

#define VMA_VECTOR_FAILFLAG_ (~(((size_t) -1) >> 1))

static inline void* vma_vector_reallocdata_(void* ptr, size_t count, size_t size, size_t* restrict pcap,
    size_t* restrict psize)
{
    void* n = mem_realloc(ptr, size);
    if (!n) {
        *pcap |= VMA_VECTOR_FAILFLAG_;
        return ptr;
    }
    *pcap = count;
    *psize = vma_vector_min_(*psize, count);
    return n;
}

static inline bool vma_vector_test_and_reset_failflag_(size_t *pcap) {
    if (*pcap & VMA_VECTOR_FAILFLAG_) {
        *pcap &= ~VMA_VECTOR_FAILFLAG_;
        return true;
    }
    return false;
}

#define vma_vector_realloc_(pv, new_cap) (                                                                   \
    (pv)->data = vma_vector_reallocdata_((pv)->data, new_cap, sizeof(*(pv)->data), &(pv)->cap, &(pv)->size), \
    !vma_vector_test_and_reset_failflag_(&(pv)->cap)                                                         \
)

#define vma_vector_resize_(pv, new_cap) ( \
    (pv)->cap == (new_cap) ||  \
    ( \
        (new_cap) > 0 ? vma_vector_realloc_(pv, new_cap) : (vma_vector_clear(pv), true) \
    ) \
)

static inline size_t vma_vector_growsize_(size_t value) {
    // integer multiplication by 1.5
    return value + (value >> 1);
}

#define vma_vector_max_cap_(pv) (SIZE_MAX / 2 / sizeof(*(pv)->data))

#define vma_vector_reserve(pv, min_cap) \
    vma_vector_reserve_internal_(pv, vma_vector_max_(min_cap, VMA_VECTOR_MINCAP_))

#define vma_vector_reserve_internal_(pv, min_cap) ( \
    (min_cap) <= (pv)->cap ((min_cap) <= vma_vector_max_cap_(pv) && \
        vma_vector_realloc_(pv, \
            vma_vector_between_(vma_vector_growsize_((pv)->cap), \
                min_cap, vma_vector_max_cap_(pv)) \
        ) \
    ) \
)

#endif