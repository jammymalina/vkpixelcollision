#include "function_loader.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "./functions.h"
#include "../../logger/logger.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define GLOBAL_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name, extension)              \
    PFN_##name name = NULL;

#include "list.inl"

void load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc) {
    if (!vk_get_proc) {
        exit(EXIT_FAILURE);
    }
    vkGetInstanceProcAddr = vk_get_proc;
}

void load_global_functions() {
    #define GLOBAL_LEVEL_VK_FUNCTION(name)                                    \
        name = (PFN_##name) vkGetInstanceProcAddr(NULL, #name);               \
        if(name == NULL) {                                                    \
            log_error("Could not load global level function: " #name);        \
            exit(EXIT_FAILURE);                                               \
        } else {                                                              \
            log_debug("Successfully loaded global Vulkan function: " #name);  \
        }

    #include "list.inl"
}

void load_instance_vulkan_functions(VkInstance instance)
{
    #define INSTANCE_LEVEL_VK_FUNCTION(name)                                  \
        name = (PFN_##name) vkGetInstanceProcAddr(instance, #name);           \
        if (name == NULL) {                                                   \
            log_error("Could not load instance level function: " #name);      \
            exit(EXIT_FAILURE);                                               \
        } else {                                                              \
            log_debug("Successfully loaded instance level function: " #name); \
        }

    #define INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name)                   \
        name = (PFN_##name) vkGetInstanceProcAddr(instance, #name);           \
        if (name == NULL) {                                                   \
            log_error("Could not load instance level function: " #name);      \
            exit(EXIT_FAILURE);                                               \
        } else {                                                              \
            log_debug("Successfully loaded instance level function: " #name); \
        }

    #include "list.inl"
}

void load_device_level_functions(VkDevice device) {
    #define DEVICE_LEVEL_VK_FUNCTION(name)                                    \
        name = (PFN_##name) vkGetDeviceProcAddr(device, #name);               \
        if (name == NULL) {                                                   \
            log_error("Could not load device level function: " #name);        \
            exit(EXIT_FAILURE);                                               \
        } else {                                                              \
            log_debug("Successfully loaded device level function: " #name);   \
        }

    #define DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name, extension)          \
        name = (PFN_##name) vkGetDeviceProcAddr(device, #name);               \
        if (name == NULL) {                                                   \
            log_error("Could not load device level function: " #name);        \
            exit(EXIT_FAILURE);                                               \
        } else {                                                              \
            log_debug("Successfully loaded device level function: " #name);   \
        }

    #include "list.inl"
}

