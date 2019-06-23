#ifndef HASH_STRING_MAP_H
#define HASH_STRING_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "./vector.h"

#define HASH_KEY_MAX_SIZE 64
#define HASH_KEY_AVAILABLE "_AVAIL"

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
    if (mem_cmp(key1, key2, HASH_KEY_MAX_SIZE) == 0) {
        return true;
    }
    size_t i;
    for (i = 0; i < HASH_KEY_MAX_SIZE && key1[i] == key2[i] &&
        key1[i] != '\0' && key2[i] != '\0'; ++i);
    return i >= HASH_KEY_MAX_SIZE || key1[i] == key2[i];
}

static inline bool hash_string_key_is_empty_(const char key[HASH_KEY_MAX_SIZE]){
    return hash_string_key_equal_(key, "");
}

static inline bool hash_string_key_is_avail_(const char key[HASH_KEY_MAX_SIZE]){
    return hash_string_key_equal_(key, HASH_KEY_AVAILABLE);
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

#define hash_string_map_get_size(phm) ((phm)->size)

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

#define hash_string_map_set_item_idx_(phm, idx, map_key, map_value) (void) ({  \
    hash_string_key_set_((phm)->nodes.data[idx].key, map_key);                 \
    (phm)->nodes.data[idx].value = map_value;                                  \
})

#define hash_string_map_copy_to_old_data_(phm) ({                              \
    vector_init(&(phm)->_old_nodes);                                           \
    bool _old_data_status = vector_reserve(&(phm)->_old_nodes,                 \
        (phm)->nodes.cap);                                                     \
    for (size_t _i = 0; _i < (phm)->nodes.cap && _old_data_status; ++_i) {     \
        hash_string_key_set_((phm)->_old_nodes.data[_i].key,                   \
            (phm)->nodes.data[_i].key);                                        \
        (phm)->_old_nodes.data[_i].value = (phm)->nodes.data[_i].value;        \
    }                                                                          \
    _old_data_status;                                                          \
})

#define hash_string_map_find_internal_(phm, map_key, pidx) ({                  \
    size_t _n = hash_string_map_get_capacity(phm);                             \
    size_t _find_idx = hash_string_1((map_key)) % _n;                          \
    size_t _i = 1;                                                             \
    ssize_t _find_element_idx = -1;                                            \
    bool _node_found = false;                                                  \
    while (true) {                                                             \
        if (hash_string_key_is_empty_((phm)->nodes.data[_find_idx].key)) {     \
            if (_find_element_idx == -1) {                                     \
                _find_element_idx = _find_idx;                                 \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        if (hash_string_key_is_avail_((phm)->nodes.data[_find_idx].key)) {     \
            if (_find_element_idx == -1) {                                     \
                _find_element_idx = _find_idx;                                 \
            }                                                                  \
        } else {                                                               \
            if (hash_string_key_equal_((phm)->nodes.data[_find_idx].key,       \
                map_key))                                                      \
            {                                                                  \
                _find_element_idx = _find_idx;                                 \
                _node_found = true;                                            \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        _find_idx = (_find_idx + _i * hash_string_2(map_key)) % _n;            \
        ++_i;                                                                  \
    }                                                                          \
    *(pidx) = _find_element_idx;                                               \
    _node_found;                                                               \
})

#define hash_string_map_add_nogrow(phm, map_key, map_val) ({                   \
    ssize_t _add_ng_idx = -1;                                                  \
    bool _node_found = hash_string_map_find_internal_(phm, map_key,            \
        &_add_ng_idx);                                                         \
    if (_node_found) {                                                         \
        (phm)->nodes.data[_add_ng_idx].value = map_val;                        \
    } else {                                                                   \
        hash_string_map_set_item_idx_(phm, _add_ng_idx, map_key, map_val);     \
        ++(phm)->size;                                                         \
    }                                                                          \
    true;                                                                      \
})

#define hash_string_rehash_(phm) (void) ({                                     \
    hash_string_map_clear_values(phm);                                         \
    for (size_t _i = 0; _i < (phm)->_old_nodes.cap; ++_i) {                    \
        if (!hash_string_key_is_empty_((phm)->_old_nodes.data[_i].key) &&      \
            !hash_string_key_is_avail_((phm)->_old_nodes.data[_i].key))        \
        {                                                                      \
            hash_string_map_add_nogrow(phm,                                    \
                (phm)->_old_nodes.data[_i].key,                                \
                (phm)->_old_nodes.data[_i].value);                             \
        }                                                                      \
    }                                                                          \
})

#define hash_string_map_reserve(phm, min_cap) ({                               \
    bool _res_status = true;                                                   \
    const size_t _min_cap = min_cap;                                           \
    if (hash_string_map_get_capacity(phm) < _min_cap) {                        \
        _res_status = hash_string_map_copy_to_old_data_(phm) &&                \
            vector_reserve(&(phm)->nodes, _min_cap);                           \
        (phm)->cap = (phm)->nodes.cap;                                         \
        vector_clear(&(phm)->_old_nodes);                                      \
    }                                                                          \
    _res_status && (phm)->cap >= _min_cap;                                     \
})

#define hash_string_map_has(phm, map_key) ({                                   \
    ssize_t _has_dummy_idx = -1;                                               \
    hash_string_map_find_internal_(phm, map_key, &_has_dummy_idx);             \
})

#define hash_string_map_get(phm, map_key, pval) ({                             \
    ssize_t _read_idx_found = -1;                                              \
    bool _read_node_found = hash_string_map_find_internal_(phm, map_key,       \
        &_read_idx_found);                                                     \
    if (_read_node_found) {                                                    \
        *(pval) = (phm)->nodes.data[_read_idx_found].value;                    \
    }                                                                          \
    _read_node_found;                                                          \
})

#define hash_string_map_add(phm, map_key, elem) ({                             \
    ssize_t _add_idx = -1;                                                     \
    bool _add_status = true;                                                   \
    bool _add_node_found = hash_string_map_find_internal_(phm, map_key,        \
        &_add_idx);                                                            \
    if (_add_node_found) {                                                     \
        (phm)->nodes.data[_add_idx].value = elem;                              \
    } else {                                                                   \
        hash_string_map_set_item_idx_(phm, _add_idx, map_key, elem);           \
        ++(phm)->size;                                                         \
        if (hash_string_map_cap_above_threshold_((phm)->size, (phm)->cap)) {   \
            _add_status = hash_string_map_reserve(phm,                         \
                hash_string_map_growsize_((phm)->cap));                        \
        }                                                                      \
    }                                                                          \
    _add_status;                                                               \
})

#define hash_string_map_delete(phm, map_key) ({                                \
    ssize_t _delete_idx = -1;                                                  \
    bool _delete_node_found = hash_string_map_find_internal_(phm, map_key,     \
        &_delete_idx);                                                         \
    if (_delete_node_found) {                                                  \
        hash_string_key_set_((phm)->nodes.data[_delete_idx].key,               \
            HASH_KEY_AVAILABLE);                                               \
        --(phm)->size;                                                         \
    }                                                                          \
    _delete_node_found;                                                        \
})
#endif
