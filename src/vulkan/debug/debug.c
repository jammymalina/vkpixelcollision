#include "./debug.h"

#include <stddef.h>

#include "../../logger/logger.h"
#include "../../memory/memory.h"
#include "../../string/string.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

static vk_debugger powtjsaggemi = {
    .instance = VK_NULL_HANDLE,
    .debug_callback = VK_NULL_HANDLE
};

static VkLayerProperties* get_available_validation_layers(
    uint32_t* layers_size)
{
    *layers_size = 0;
    uint32_t count = 0;
    VkResult r = vkEnumerateInstanceLayerProperties(&count, NULL);

    if (r != VK_SUCCESS) {
        log_error("Unable to get validation layers: %s",
            vulkan_result_to_string(r));
        return NULL;
    }
    if (count == 0) {
        return NULL;
    }

    VkLayerProperties* validation_layers =
        mem_alloc(count * sizeof(VkLayerProperties));
    r = vkEnumerateInstanceLayerProperties(&count, validation_layers);

    if (!validation_layers) {
        log_error("Unable to allocate validation layers");
        return NULL;
    }

    if (r != VK_SUCCESS || count == 0) {
        log_error("Unable to get validation layers: %s",
            vulkan_result_to_string(r));
        mem_free(validation_layers);
        return NULL;
    }

    *layers_size = count;
    return validation_layers;
}

static inline bool is_validation_layer_available(const char* name,
    VkLayerProperties* available_layers, uint32_t layers_size)
{
    if (layers_size == 0 || available_layers == NULL) {
        return false;
    }
    for (size_t i = 0; i < layers_size; ++i) {
        if (string_equal(name, available_layers[i].layerName)) {
            return true;
        }
    }

    return false;
}

static inline bool are_validation_layers_available(const char** names,
    uint32_t names_size, VkLayerProperties* available_layers,
    uint32_t layers_size)
{
    if (names_size == 0) {
        return true;
    }
    if (layers_size == 0 || available_layers == NULL) {
        return false;
    }
    bool success = true;
    for (size_t i = 0; i < names_size && success; ++i) {
        success &= is_validation_layer_available(names[i], available_layers,
            layers_size);
    }

    return success;
}

bool check_validation_layers(const char** names, uint32_t names_size) {
    uint32_t layers_size = 0;
    VkLayerProperties* available_layers =
        get_available_validation_layers(&layers_size);
    bool success = are_validation_layers_available(names, names_size,
        available_layers, layers_size);

    mem_free(available_layers);

    return success;
}

static inline VkDebugReportCallbackEXT setup_debug_callback(VkInstance instance,
    VkDebugReportFlagsEXT flags)
{
    VkDebugReportCallbackCreateInfoEXT debug_callback_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = flags,
        .pfnCallback = vulkan_debug_callback,
        .pUserData = NULL
    };
    VkDebugReportCallbackEXT debug_callback;
    CHECK_VK(vkCreateDebugReportCallbackEXT(instance, &debug_callback_info,
        NULL, &debug_callback));

    return debug_callback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
    uint64_t obj, size_t location, int32_t code, const char* layer_prefix,
    const char* msg, void* user_data)
{
    log_info("VALIDATION LAYER %s: %s", layer_prefix, msg);
    return VK_FALSE;
}

void init_vk_debugger(VkInstance instance) {
    #ifdef DEBUG
        powtjsaggemi.instance = instance;
        powtjsaggemi.debug_callback = setup_debug_callback(instance,
            DEFAULT_VULKAN_DEBUG_FLAFS);
    #endif
}

void destroy_vk_debugger(VkInstance instance) {
    #ifdef DEBUG
        vkDestroyDebugReportCallbackEXT(powtjsaggemi.instance,
            powtjsaggemi.debug_callback, NULL);
    #endif
}

vk_debugger* retrieve_vk_debugger() {
    return &powtjsaggemi;
}
