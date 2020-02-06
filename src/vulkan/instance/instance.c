#include "./instance.h"

#include <SDL2/SDL_vulkan.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../memory/memory.h"
#include "../../logger/logger.h"
#include "../debug/debug.h"
#include "../functions/functions.h"
#include "../tools/tools.h"

VkInstance create_instance(SDL_Window* w) {
    const char** extensions = NULL;
    unsigned int extension_count = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(w, &extension_count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    unsigned int debug_extension_count = 0;
    #ifdef DEBUG
        debug_extension_count = 1;
    #endif

    extensions = mem_alloc(sizeof(const char*) *
        (extension_count + debug_extension_count));
    CHECK_ALLOC(extensions, "Failed to allocate memory for vulkan extensions");

    if (!SDL_Vulkan_GetInstanceExtensions(w, &extension_count, extensions)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
        extension_count += debug_extension_count;

        const char* validation_layer_names[] = {
            "VK_LAYER_LUNARG_standard_validation"
        };
        uint32_t validation_layers_size =
            sizeof(validation_layer_names) / sizeof(const char*);

        if (!check_validation_layers(validation_layer_names,
            validation_layers_size))
        {
            log_warning("Unavailable validation layers");
            validation_layers_size = 0;
        }

        extensions[extension_count - 1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
    #else
        const char** validation_layer_names = NULL;
        uint32_t validation_layers_size = 0;
    #endif

    VkApplicationInfo vk_application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Collision app",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "jammyengine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2
    };

    VkInstanceCreateInfo vk_instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &vk_application_info,
        .enabledLayerCount = validation_layers_size,
        .ppEnabledLayerNames = validation_layer_names,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions
    };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&vk_instance_create_info,
        NULL, &instance);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
        log_error("Could not create Vulkan instance: %s",
            vulkan_result_to_string(result));
        exit(EXIT_FAILURE);
    }

    mem_free(extensions);

    return instance;
}

void destroy_instance(VkInstance instance) {
    vkDestroyInstance(instance, NULL);
}
