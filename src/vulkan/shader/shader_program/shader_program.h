#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../vertex/core/vertex_layout.h"
#include "./pipeline_state.h"
#include "./shader_types.h"

#define SHADER_PROGRAM_PIPELINE_CACHE_SIZE 64

typedef struct shader_program_create_info {
} shader_program_create_info;

typedef struct shader_program {
    VkDevice device;
    VkRenderPass render_pass;
    VkPipelineCache vk_pipeline_cache;

    const shader* shaders[SHADER_TYPES_COUNT];

    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
    vertex_layout_type vertex_layout;

    pipeline_state pipeline_cache[SHADER_PROGRAM_PIPELINE_CACHE_SIZE];
    size_t pipeline_cache_size;

} shader_program;

void shader_program_init_empty(shader_program* prog);

bool shader_program_has_pipeline(const shader_program* prog, pipeline_state_bits
    state_bits);

void shader_program_remove_least_used_pipeline_from_cache(shader_program*
    prog);
void shader_program_add_pipeline_to_cache(shader_program* prog, const
    pipeline_state* ps);

const pipeline_state* shader_program_get_pipeline_by_state_bits(shader_program*
    prog, pipeline_state_bits state_bits);

void shader_program_destroy(shader_program* prog);

#endif
