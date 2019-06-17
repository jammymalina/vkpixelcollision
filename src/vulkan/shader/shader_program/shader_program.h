#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <vulkan/vulkan.h>

#include "./shader_types.h"

typedef struct shader_program {
    VkDevice device;
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
} shader_program;

void shader_program_init_empty(shader_program* prog);
void shader_program_destroy(shader_program* prog);

#endif
