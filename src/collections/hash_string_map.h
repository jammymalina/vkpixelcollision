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
    size_t cap;                                                                \
    size_t size;                                                               \
    struct VECTOR(struct {                                                     \
        char key[HASH_KEY_MAX_SIZE];                                           \
        type value;                                                            \
    }) _old_nodes;                                                             \
}

#define hash_string_map_init(phm) (void) ({                                    \
    vector_init(&(phm)->nodes);                                                \
    vector_init(&(phm)->_old_nodes);                                           \
    (phm)->cap = 0;                                                            \
    (phm)->size = 0;                                                           \
})

#define hash_string_map_destroy(phm) vector_destroy(&(phm)->nodes)

#define hash_string_map_clear(phm) vector_clear(&(phm)->nodes)

#define hash_string_map_get_capacity(phm) ((phm)->cap)

#define hash_string_map_get_size(phm) ((phm)->nodes.size)

#define hash_string_map_clear_values(phm) (void) ({                            \
    for (size_t _idx = 0; _idx < hash_string_map_get_capacity(phm); ++_idx) {  \
        hash_string_key_clear_((phm)->nodes.data[_idx].key);                   \
    }                                                                          \
})

static inline size_t hash_string_map_growsize_(size_t value) {
    // integer multiplication by 2
    return value << 1;
}

static inline bool hash_string_map_cap_above_threshold_(size_t size, size_t
    capacity)
{
    return (3 * size) > (capacity << 1);
}

#define hash_string_map_set_item_idx_(phm, idx, key, value) (void) ({          \
    hash_string_key_set_((phm)->nodes.data[idx].key, key);                     \
    (phm)->nodes.data[idx].value = value;                                      \
})

#define hash_string_map_find_internal_(phm, key, pidx) ({                      \
    size_t _n = hash_string_map_get_capacity(phm);                             \
    size_t _idx = hash_string_1((key)) % _n;                                   \
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
        _idx = (_idx + _i * hash_string_2(key)) % _n;                          \
        ++_i;                                                                  \
    }                                                                          \
    _node_found;                                                               \
})

#define hash_string_map_add_nogrow(phm, key, value) ({                         \
    ssize_t _idx = -1;                                                         \
    bool _node_found = hash_string_map_find_internal_(phm, key, &_idx);        \
    if (_node_found) {                                                         \
        (phm)->nodes.data[_idx].value = value;                                 \
    } else {                                                                   \
        hash_string_map_set_item_idx_(phm, _idx, key, value);                  \
        ++(phm)->size;                                                         \
    }                                                                          \
    true;                                                                      \
})

#define hash_string_rehash_(phm, old_cap) ({                                   \
    vector_init(&(phm)->_old_nodes);                                           \
    bool _rehash_status = vector_reserve(&(phm)->_old_nodes,                   \
        (phm)->nodes.size);                                                    \
    for (size_t _i = 0; _i < (phm)->nodes.size; ++_i) {                        \
        hash_string_key_set_((phm)->_old_nodes.data[_i].key,                   \
            (phm)->nodes.data[_i].key);                                        \
        (phm)->_old_nodes.data[_i].value = (phm)->nodes.data[_i].value;        \
    }                                                                          \
    if (_rehash_status) {                                                      \
        hash_string_map_clear_values(phm);                                     \
        for (size_t _i = 0; _i < (old_cap); ++_i) {                            \
            if (!hash_string_key_is_empty_((phm)->_old_nodes.data[_i].key) &&  \
                !hash_string_key_is_avail_((phm)->_old_nodes.data[_i].key))    \
            {                                                                  \
                hash_string_map_add_nogrow(phm,                                \
                    (phm)->_old_nodes.data[_i].key,                            \
                    (phm)->_old_nodes.data[_i].value);                         \
            }                                                                  \
        }                                                                      \
    }                                                                          \
    vector_clear(&(phm)->_old_nodes);                                          \
    _rehash_status;                                                            \
})

#define hash_string_map_reserve(phm, min_cap) ({                               \
    bool _res_status = true;                                                   \
    size_t _old_cap = (phm)->cap;                                              \
    if (hash_string_map_get_capacity(phm) < min_cap) {                         \
        _res_status = vector_reserve(&(phm)->nodes, min_cap) &&                \
            ((phm)->cap = vector_max_(min_cap, (phm)->nodes.cap)) &&           \
            hash_string_rehash_(phm, _old_cap);                                \
    }                                                                          \
    _res_status;                                                               \
})

#define hash_string_map_has(phm, key) ( \
    ssize_t _idx = -1, \
    hash_string_map_find_internal_(phm, key, &_idx) \
)

#define hash_string_map_get(phm, key, pval) ({                                 \
    ssize_t _idx = -1;                                                         \
    bool _node_found = hash_string_map_find_internal_(phm, key, &_idx);        \
    if (_node_found) {                                                         \
        *(pval) = (phm)->nodes.data[_idx].value;                               \
    }                                                                          \
    _node_found;                                                               \
})

#define hash_string_map_add(phm, key, value) ({                                \
    ssize_t _idx = -1;                                                         \
    bool _res_status = true;                                                   \
    bool _node_found = hash_string_map_find_internal_(phm, key, &_idx);        \
    if (_node_found) {                                                         \
        (phm)->nodes.data[_idx].value = value;                                 \
    } else {                                                                   \
        hash_string_map_set_item_idx_(phm, _idx, key, value);                  \
        ++(phm)->size;                                                         \
        if (hash_string_map_cap_above_threshold_((phm)->size, (phm)->cap)) {   \
            _res_status = hash_string_map_reserve(phm,                         \
                hash_string_map_growsize_((phm)->cap));                        \
        }                                                                      \
    }                                                                          \
    _res_status;                                                               \
})

#define hash_string_map_delete(phm, key) ({                                    \
    ssize_t _idx = -1;                                                         \
    bool _node_found = hash_string_map_find_internal_(phm, key, &_idx);        \
    if (_node_found) {                                                         \
        hash_string_key_set_((phm)->nodes.data[_idx].key, HASH_KEY_AVAILABLE); \
        --(phm)->size;                                                         \
    }                                                                          \
    _node_found;                                                               \
})
#endif
