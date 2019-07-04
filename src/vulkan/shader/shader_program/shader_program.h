#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../vertex/core/vma_types.h"
#include "./pipeline_state.h"
#include "./shader_types.h"

#define PIPELINE_CACHE_SIZE 64

typedef struct shader_program {
    VkDevice device;

    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
    vertex_layout_type vertex_layout;

    pipeline_state pipeline_cache[PIPELINE_CACHE_SIZE];
    size_t pipeline_cache_size;

    char shaders[SHADER_TYPES_COUNT][SHADER_MAX_NAME_SIZE];
} shader_program;

void shader_program_init_empty(shader_program* prog);

uint32_t shader_program_get_shader_type_bits(const shader_program* prog);

void shader_program_destroy(shader_program* prog);

#endif
