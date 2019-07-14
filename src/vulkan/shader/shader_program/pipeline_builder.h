#ifndef PIPELINE_BUILDER_H
#define PIPELINE_BUILDER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./pipeline_state.h"
#include "./shader_types.h"
#include "./shader.h"
#include "./shader_program.h"

typedef struct pipeline_create_info {
    pipeline_state_bits state_bits;
    VkRenderPass render_pass;
    bool store_in_program_cache;
} pipeline_create_info;

bool pipeline_builder_build_pipeline(pipeline_state* result,
    shader_program* prog, const pipeline_create_info* pipe_info);

#endif
