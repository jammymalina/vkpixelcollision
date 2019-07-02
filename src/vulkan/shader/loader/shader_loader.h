#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <stddef.h>

#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"

#define DEFAULT_MAX_SHADER_PROGRAM_BYTE_SIZE (1024 * 1024)

typedef struct shader_loader_create_info {
    size_t max_shader_program_byte_size;
} shader_loader_create_info;

typedef struct shader_loader {
    char* program_buffer;
    size_t max_shader_program_byte_size;
} shader_loader;

void shader_loader_init_empty(shader_loader* shl);
bool shader_loader_init(shader_loader* shl, const shader_loader_create_info*
    shl_config);

bool shader_loader_load_shader(const shader_loader* shl, shader* shd, const
    char* filename, const char name[SHADER_MAX_NAME_SIZE], const gpu_info* gpu);

void shader_loader_destroy(shader_loader* loader);

#endif
