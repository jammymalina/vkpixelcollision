#include "./shader.h"

#include "../../functions/functions.h"
#include "../../../logger/logger.h"


void shader_init_empty(shader* shd) {
    shd->module = VK_NULL_HANDLE;
    shd->device = VK_NULL_HANDLE;
    shd->type = SHADER_TYPE_UNDEFINED;
    shd->bindings_size = 0;
}

static VkShaderModule shader_create_vk_module(const shader_create_info
    *shader_info)
{
    VkShaderModule module = VK_NULL_HANDLE;

    const VkShaderModuleCreateInfo module_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader_info->program_size,
        .pCode = shader_info->program
    };

    const VkResult module_status = vkCreateShaderModule(shader_info->device,
        &module_info, NULL, &module);
    if (module_status != VK_SUCCESS) {
        log_error("Unable to create shader module");
        return VK_NULL_HANDLE;
    }

    return module;
}

bool shader_init(shader* shd, const shader_create_info* shader_info) {
    shader_init_empty(shd);

    shd->type = shader_info->type;
    shd->device = shader_info->device;

    const VkShaderModule module = shader_create_vk_module(shader_info);
    if (!module) {
        log_error("Unable to create shader");
        return false;
    }

    shd->module = module;

    return true;
}

void shader_copy(shader* dest, const shader* src) {
    dest->type = src->type;
    dest->module = src->module;
    dest->device = src->device;
    dest->bindings_size = src->bindings_size;
    mem_copy(dest->bindings, src->bindings, sizeof(shader_binding) *
        SHADER_MAX_BINDINGS_SIZE);
}

void shader_destroy(shader* shd) {
    if (shd->module && shd->device) {
        log_info("Destroying shader");
        vkDestroyShaderModule(shd->device, shd->module, NULL);
        shd->module = VK_NULL_HANDLE;
    }
}
