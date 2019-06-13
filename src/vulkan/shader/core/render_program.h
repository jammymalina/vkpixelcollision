#ifndef RENDER_PROGRAM_H
#define RENDER_PROGRAM_H

#include <vulkan/vulkan.h>

#include "./shader_types.h"

typedef struct render_program {
    char name[SHADER_MAX_NAME_SIZE];

    shader_vector shaders;

    VkDevice device;
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
} render_program;

void render_program_init_empty(render_program* prog);
void render_program_destroy(render_program* prog);

#endif
