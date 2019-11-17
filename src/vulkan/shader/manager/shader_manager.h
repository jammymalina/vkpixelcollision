#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../collections/hash_string_map.h"
#include "../../../file/path.h"
#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader.h"

#define DEFAULT_EXPECTED_NUMBER_OF_SHADERS 10

typedef struct shader_hash_map HASH_STRING_MAP(shader) shader_hash_map;

typedef struct shader_manager_create_info {
    size_t expected_number_of_shaders;
} shader_manager_create_info;

typedef struct shader_preload_item {
    const char* filepath;
    const char* name;
    shader_binding bindings[SHADER_MAX_BINDINGS_SIZE];
    size_t bindings_size;
} shader_preload_item;

typedef struct shader_preload_info {
    const shader_preload_item* preloaded_shaders;
    size_t preloaded_shaders_size;
} shader_preload_info;

typedef struct shader_manager {
    shader_hash_map shaders;
} shader_manager;

void shader_manager_init_empty(shader_manager* shm);
bool shader_manager_init(shader_manager* shm, const shader_manager_create_info*
    shm_info);

bool shader_manager_add(shader_manager* shm, const char* shader_name, const
    shader* program);
bool shader_manager_get(shader_manager* shm, const char* shader_name, shader*
    program);
shader* shader_manager_get_reference(const shader_manager* shm, const char*
    shader_name);
bool shader_manager_has(shader_manager* shm, const char* shader_name);
bool shader_manager_delete(shader_manager* shm, const char* shader_name);

bool shader_manager_preload(shader_manager* shm, const shader_preload_info*
    preload_info, const char basepath[PATH_MAX_SIZE], const gpu_info* gpu);

void shader_manager_destroy(shader_manager* shm);

#endif
