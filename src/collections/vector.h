#ifndef COL_VECTOR_H
#define COL_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../../memory/memory.h"

#define COL_VECTOR(type) {                                                     \
    size_t cap;                                                                \
    size_t size;                                                               \
    type* data;                                                                \
}

#define COL_VECTOR_INITIALIZER { 0, 0, NULL }

#define col_vector_init(pv) (void) (                                           \
    (pv)->cap = 0,                                                             \
    (pv)->size = 0,                                                            \
    (pv)->data = NULL                                                          \
)

#define col_vector_destroy(pv) free((pv)->data)

#define col_vector_clear(pv) (                                                 \
    col_vector_destroy(pv),                                                    \
    col_vector_init(pv)                                                        \
)

#define COL_VECTOR_MINCAP_ ((size_t) 10)

static inline size_t col_vector_min_(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline size_t col_vector_max_(size_t a, size_t b) {
    return a > b ? a : b;
}

static inline size_t col_vector_between_(size_t x, size_t min, size_t max) {
    return col_vector_max_(min, col_vector_min_(max, x));
}

static inline size_t col_vector_enforce_size_t_(size_t value) {
    return value;
}

#define COL_VECTOR_FAILFLAG_ (~(((size_t) -1) >> 1))

static inline void* col_vector_reallocdata_(void* ptr, size_t count,
    size_t size, size_t* restrict pcap, size_t* restrict psize)
{
    void* n = mem_realloc(ptr, size);
    if (!n) {
        *pcap |= COL_VECTOR_FAILFLAG_;
        return ptr;
    }
    *pcap = count;
    *psize = col_vector_min_(*psize, count);
    return n;
}

static inline bool col_vector_test_and_reset_failflag_(size_t *pcap) {
    if (*pcap & COL_VECTOR_FAILFLAG_) {
        *pcap &= ~COL_VECTOR_FAILFLAG_;
        return true;
    }
    return false;
}

#define col_vector_realloc_(pv, new_cap) (                                     \
    (pv)->data = col_vector_reallocdata_((pv)->data, new_cap,                  \
        sizeof(*(pv)->data), &(pv)->cap, &(pv)->size),                         \
    !col_vector_test_and_reset_failflag_(&(pv)->cap)                           \
)

#define col_vector_resize_(pv, new_cap) (                                      \
    (pv)->cap == (new_cap) || (                                                \
        (new_cap) > 0 ?                                                        \
            col_vector_realloc_(pv, new_cap) : (col_vector_clear(pv), true)    \
    )                                                                          \
)

static inline size_t col_vector_growsize_(size_t value) {
    // integer multiplication by 1.5
    return value + (value >> 1);
}

#define col_vector_max_cap_(pv) (SIZE_MAX / 2 / sizeof(*(pv)->data))

#define col_vector_reserve(pv, min_cap)                                        \
    col_vector_reserve_internal_(pv,                                           \
        col_vector_max_(min_cap, COL_VECTOR_MINCAP_))

#define col_vector_reserve_internal_(pv, min_cap) ((min_cap) <= (pv)->cap || ( \
    (min_cap) <= col_vector_max_cap_(pv) &&                                    \
    col_vector_realloc_(pv,                                                    \
        col_vector_between_(col_vector_growsize_((pv)->cap), min_cap,          \
        col_vector_max_cap_(pv))))                                             \
)

#define col_vector_shrink_to_fit(pv) (void)                                    \
    col_vector_resize_(pv, (pv)->size)

#define col_vector_autoshrink(pv) (void) (                                     \
    (pv)->cap <= COL_VECTOR_MINCAP_ ||                                         \
    (pv)->cap < col_vector_growsize_((pv)->size+5) ||                          \
    col_vector_resize_(pv, col_vector_max_((pv)->size+5, COL_VECTOR_MINCAP_))  \
)

#define col_vector_push(pv, item) (                                            \
    col_vector_reserve(pv, (pv)->size + 1) && (                                \
        (pv)->data[(pv)->size++] = (item),                                     \
        true                                                                   \
    )                                                                          \
)

#define col_vector_push_all(pv, items, count)                                  \
    col_vector_push_all_internal_(pv, items, col_vector_enforce_size_t_(count))

#define col_vector_push_all_internal_(pv, items, count) (                      \
    col_vector_check_same_ptr_type_((pv)->data, items),                        \
    col_vector_reserve(pv, (pv)->size + (count)) && (                          \
        memcpy(&(pv)->data[(pv)->size], items, (count) * sizeof(*(pv)->data)), \
        (pv)->size += (count),                                                 \
        true                                                                   \
    )                                                                          \
)

#define col_vector_insert_hole(pv, index, count)                               \
    col_vector_insert_hole_internal_(pv, col_vector_enforce_size_t_(index),    \
        col_vector_enforce_size_t_(count))

#define col_vector_insert_hole_internal_(pv, index, count) (                   \
    col_vector_reserve(pv, (pv)->size + (count)) && (                          \
        (index) == (pv)->size || (                                             \
            mem_move(&(pv)->data[(index) + (count)], &(pv)->data[index],       \
                ((pv)->size - (index)) * sizeof(*(pv)->data)),                 \
            true                                                               \
        )                                                                      \
    ) && (                                                                     \
        (pv)->size += (count),                                                 \
        true                                                                   \
    )                                                                          \
)

#define col_vector_insert(pv, index, item) (                                   \
    col_vector_insert_hole(pv, index, 1) && (                                  \
        (pv)->data[index] = (item),                                            \
        true                                                                   \
    )                                                                          \
)

#define col_vector_insert_all(pv, index, items, count) (                       \
    col_vector_check_same_ptr_type_((pv)->data, items),                        \
    col_vector_insert_hole(pv, index, count) && (                              \
        memcpy(&(pv)->data[index], items, (count) * sizeof(*(pv)->data)),      \
        true                                                                   \
    )                                                                          \
)

static inline void col_vector_reverse_array_(char* array, size_t len) {
    for (size_t i = 0; i < len / 2; ++i) {
        char c = array[i];
        array[i] = array[len - i - 1];
        array[len - i - 1] = c;
    }
}

static inline void col_vector_rotate_array_left_(char* array, size_t len,
    size_t distance)
{
    col_vector_reverse_array_(array, distance);
    col_vector_reverse_array_(&array[distance], len - distance);
    col_vector_reverse_array_(array, len);
}

static inline void col_vector_rotate_array_right_(char* array, size_t len,
    size_t distance)
{
    col_vector_rotate_array_left_(array, len, len - distance);
}

static inline void col_vector_move_(char* array, size_t index, size_t count,
    size_t target)
{
    if (index < target) {
        col_vector_rotate_array_left_(&array[index], target - index + count,
            count);
    } else {
        col_vector_rotate_array_right_(&array[target], index - target + count,
            count);
    }
}

#define col_vector_move_slice(pv, index, count, target)                        \
    col_vector_move_slice_internal_(pv,                                        \
        col_vector_enforce_size_t_(index),                                     \
        col_vector_enforce_size_t_(count),                                     \
        col_vector_enforce_size_t_(target)                                     \
    )

#define col_vector_move_slice_internal_(pv, index, count, target)              \
    col_vector_move_((char *) (pv)->data,                                      \
        (index) * sizeof((pv)->data[0]),                                       \
        (count) * sizeof((pv)->data[0]),                                       \
        (target) * sizeof((pv)->data[0])                                       \
    )

#define col_vector_move(pv, index, target)                                     \
    col_vector_move_slice(pv, index, 1, target)

#define col_vector_remove_slice_noshrink(pv, index, count)                     \
    col_vector_remove_slice_noshrink_internal_(pv,                             \
        col_vector_enforce_size_t_(index),                                     \
        col_vector_enforce_size_t_(count)                                      \
    )

#define col_vector_remove_slice_noshrink_internal_(pv, index, count)           \
    do {                                                                       \
        if ((index) + (count) < (pv)->size)                                    \
            memmove(&(pv)->data[index],                                        \
                    &(pv)->data[(index) + (count)],                            \
                    ((pv)->size - (index) - (count)) * sizeof(*(pv)->data));   \
        (pv)->size -= (count);                                                 \
    } while (0)

#define col_vector_remove_slice(pv, index, count)                              \
    do {                                                                       \
        col_vector_remove_slice_noshrink(pv, index, count);                    \
        col_vector_autoshrink(pv);                                             \
    } while (0)

#define col_vector_remove_noshrink(pv, index)                                  \
    col_vector_remove_slice_noshrink(pv, index, 1)

#define col_vector_remove(pv, index)                                           \
    do {                                                                       \
        col_vector_remove_noshrink(pv, index);                                 \
        col_vector_autoshrink(pv);                                             \
    } while (0)

#define col_vector_swap_remove(pv, index)                                      \
    (pv)->data[index] = (pv)->data[--(pv)->size]

#define col_vector_index_of(pv, item, pidx)                                    \
    do {                                                                       \
        ssize_t* out = pidx;                                                   \
        size_t col_vector_find_idx_;                                           \
        for (col_vector_find_idx_ = 0; col_vector_find_idx_ < (pv)->size &&    \
            ((pv)->data[col_vector_find_idx_] != (item));                      \
            ++col_vector_find_idx_);                                           \
        *out = col_vector_find_idx_ == (pv)->size ? -1 :                       \
            (ssize_t) col_vector_find_idx_;                                    \
    } while (0)

#define col_vector_foreach(item, pv)                                           \
    for (size_t col_vector_idx_##item = 0;                                     \
        col_vector_idx_##item < (pv)->size &&                                  \
            ((item) = (pv)->data[col_vector_idx_##item], true);                \
         ++col_vector_idx_##item)

#endif
