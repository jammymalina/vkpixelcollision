#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#define DEFAULT_VULKAN_DEBUG_FLAFS \
    (VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | \
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)

bool check_validation_layers(const char** names, uint32_t names_size);

typedef struct vk_debugger {
    VkInstance instance;
    VkDebugReportCallbackEXT debug_callback;
} vk_debugger;

void init_vk_debugger(VkInstance instance);
void destroy_vk_debugger();
vk_debugger* retrieve_vk_debugger();

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
    uint64_t obj, size_t location, int32_t code, const char* layer_prefix,
    const char* msg, void* user_data);

#endif
