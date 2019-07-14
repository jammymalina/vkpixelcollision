#ifndef SHADER_PROGRAM_BUILDER_H
#define SHADER_PROGRAM_BUILDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../gpu/gpu.h"
#include "../shader_program/shader_types.h"
#include "../shader_program/shader_program.h"

typedef struct shader_builder_program_create_info {
    const gpu_info* gpu;
    const char** shaders;
    size_t shaders_size;

    VkRenderPass render_pass;

    pipeline_state_bits state_bits;
} shader_builder_program_create_info;

bool shader_builder_preload_programs(shader_program* prog, const
    shader_manager_program_create_info* prog_info);

bool shader_builder_build_program(shader_program* prog, pipeline_state*
    result_pipeline, const shader_builder_program_create_info* prog_info);

#endif
