#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#define DEFAULT_VULKAN_DEBUG_FLAFS (VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT)

bool check_validation_layers(const char **names, uint32_t names_size);

VkLayerProperties *get_available_validation_layers(uint32_t *layers_size);
bool is_validation_layer_available(const char *name, VkLayerProperties *available_layers, uint32_t layers_size);
bool are_validation_layers_available(const char **names,
    uint32_t names_size, VkLayerProperties *available_layers, uint32_t layers_size);

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layer_prefix,
    const char *msg, void *userData);

VkDebugReportCallbackEXT setup_debug_callback(VkInstance instance, VkDebugReportFlagsEXT flags);

#endif
