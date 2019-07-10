#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "./shader_types.h"

#define SHADER_MAX_BINDINGS_SIZE 16

typedef struct shader_create_info {
    shader_type type;
    uint32_t* program;
    size_t program_size;

    VkDevice device;
} shader_create_info;

typedef struct shader {
    shader_type type;

    VkShaderModule module;
    VkDevice device;

    shader_binding bindings[SHADER_MAX_BINDINGS_SIZE];
    size_t bindings_size;
} shader;

void shader_init_empty(shader* shd);
bool shader_init(shader* shd, const shader_create_info* shader_info);
void shader_copy(shader* dest, const shader* src);
void shader_destroy(shader* shd);

#endif
