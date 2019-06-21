#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../collections/hash_string_map.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader.h"

#define DEFAULT_EXPECTED_NUMBER_OF_SHADERS 10

typedef struct shader_hash_map HASH_STRING_MAP(shader) shader_hash_map;

typedef struct shader_manager_create_info {
size_t expected_number_of_shaders;
} shader_manager_create_info;

typedef struct shader_manager {
    shader_hash_map shaders;
} shader_manager;

void shader_manager_init_empty(shader_manager* shm);
bool shader_manager_init(shader_manager* shm, const shader_manager_create_info*
    shm_info);
void shader_manager_destroy(shader_manager* shm);

#endif
