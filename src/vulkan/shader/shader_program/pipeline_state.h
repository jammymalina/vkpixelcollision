#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./shader_types.h"
#include "../../gpu/gpu.h"

typedef struct pipeline_state {
    VkPipeline handle;
    VkRenderPass render_pass;
    pipeline_state_bits state_bits;
    const gpu_info* gpu;
    size_t counter;
} pipeline_state;

void pipeline_state_init_empty(pipeline_state* ps);
void pipeline_state_copy(pipeline_state* dest, const pipeline_state* src);

VkPipelineRasterizationStateCreateInfo pipeline_state_get_rasterization_state(
    const pipeline_state* ps);
VkPipelineColorBlendAttachmentState pipeline_state_get_color_blend_attachment(
    const pipeline_state* ps);
VkPipelineDepthStencilStateCreateInfo pipeline_state_get_depth_stencil_info(
    const pipeline_state* ps);
VkPipelineMultisampleStateCreateInfo pipeline_state_get_multisample_info(const
    pipeline_state* ps);
void pipeline_state_get_dynamic_states(const pipeline_state* ps, VkDynamicState*
    dynamic_states, size_t* dynamic_states_size);

void pipeline_state_destroy(pipeline_state* ps);

#endif
