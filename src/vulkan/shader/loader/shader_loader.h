#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"

bool shader_loader_load_shader(shader* shd, const char* filename,
    const char name[SHADER_MAX_NAME_SIZE], const gpu_info* gpu);

#endif
