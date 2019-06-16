#ifndef HASH_STRING_MAP_H
#define HASH_STRING_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "./vector.h"

#define HASH_KEY_MAX_SIZE 64
#define HASH_KEY_AVAILABLE "_AVAIL"

// djb2
static inline unsigned long hash_string_1(const char key[HASH_KEY_MAX_SIZE]) {
    unsigned long hash = 5381;

    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// sdbm
static inline unsigned long hash_string_2(const char key[HASH_KEY_MAX_SIZE]) {
    unsigned long hash = 0;

    int c;
    while ((c = *key++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

static inline void hash_string_key_clear_(char key[HASH_KEY_MAX_SIZE]) {
    mem_set(key, '\0', HASH_KEY_MAX_SIZE);
}

static inline void hash_string_key_set_(char key[HASH_KEY_MAX_SIZE], const char*
    key_value)
{
    hash_string_key_clear_(key);

    for (size_t i = 0; i < HASH_KEY_MAX_SIZE; ++i) {
        key[i] = key_value[i];
        if (key_value[i] == '\0') {
            break;
        }
    }
}

static inline bool hash_string_key_equal_(const char key1[HASH_KEY_MAX_SIZE],
    const char key2[HASH_KEY_MAX_SIZE])
{
    return mem_cmp(key1, key2, HASH_KEY_MAX_SIZE) == 0;
}

static inline bool hash_string_key_is_empty_(const char key[HASH_KEY_MAX_SIZE]){
    return hash_string_key_equal_(key, "");
}

static inline bool hash_string_key_is_avail_(const char key[HASH_KEY_MAX_SIZE]){
    return hash_string_key_equal_(key, HASH_KEY_AVAILABLE);
}

#define HASH_STRING_MAP(type) {                                                \
    struct VECTOR(struct {                                                     \
        char key[HASH_KEY_MAX_SIZE];                                           \
        type value;                                                            \
    }) nodes;                                                                  \
    type not_found_value;                                                      \
}

#define hash_string_map_init(phm, default_value) (void) (                      \
    vector_init(&(phm)->nodes),                                                \
    (phm)->not_found_value = (default_value)                                   \
)

#define hash_string_map_destroy(phm) vector_destroy(&(phm)->nodes)

#define hash_string_map_clear(phm) vector_clear(&(phm)->nodes)

#define hash_string_map_get_capacity(phm) ((phm)->nodes.cap)

#define hash_string_map_get_size(phm) ((phm)->nodes.size)

#define hash_string_map_clear_values_(phm) (void) ({                           \
    for (size_t _idx = 0; _idx < hash_string_map_get_capacity(phm); ++_idx) {  \
        (phm)->nodes.data[_idx].value = (phm)->not_found_value;                \
        hash_string_key_clear_((phm)->nodes.data[_idx].key);                   \
    }                                                                          \
})

#define hash_string_map_reserve(phm, min_cap) (                                \
    vector_reserve(&(phm)->nodes, min_cap) && (                                \
        hash_string_map_clear_values_(phm),                                    \
        true                                                                   \
    )                                                                          \
)

#define hash_string_map_find_internal_(phm, key, pidx) ({                      \
    size_t n = (phm)->nodes.capacity;                                          \
    size_t _idx = hash_string_1((key)) % n;                                    \
    size_t _i = 1;                                                             \
    *(pidx) = -1;                                                              \
    bool _node_found = false;                                                  \
    while (true) {                                                             \
        if (hash_string_key_is_empty_((phm)->nodes.data[_idx].key)) {          \
            if (*(pidx) == -1) {                                               \
                *(pidx) = _idx;                                                \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        if (hash_string_key_is_avail_((phm)->nodes.data[_idx].key)) {          \
            if (*(pidx) == -1) {                                               \
                *(pidx) = _idx;                                                \
            }                                                                  \
        } else {                                                               \
            if (hash_string_key_equal_((phm)->nodes.data[_idx].key, key)) {    \
                _node_found = true;                                            \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        _idx = (_idx + _i * hash_string_2(key));                               \
        ++_i;                                                                  \
    }                                                                          \
    _node_found                                                                \
})

#define hash_string_map_add(phm, key, value) ({                                \
    ssize_t _idx = -1;                                                         \
    bool _node_found = hash_string_map_find_internal_(phm, key, &_idx);        \
    \
})

#endif
