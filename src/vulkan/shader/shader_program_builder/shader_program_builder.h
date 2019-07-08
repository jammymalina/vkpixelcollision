#ifndef SHADER_PROGRAM_BUILDER_H
#define SHADER_PROGRAM_BUILDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader_program.h"

typedef struct shader_program_create_info {
    char* name;

    gpu_info* gpu;
    char** shaders;
    size_t shaders_size;

    pipeline_state_bits* preconfigured_pipelines;
    size_t preconfigured_pipelines_size;
} shader_program_create_info;

typedef struct shader_program_preload_info {
    shader_program_create_info* shader_programs_config;
    size_t shader_programs_config_size;
} shader_program_preload_info;

bool shader_builder_build_shader_program(shader_program* prog, const
    shader_program_create_info* prog_info);

#endif
