#ifndef VULKAN_TOOLS_H
#define VULKAN_TOOLS_H

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "../../logger/logger.h"

#define CHECK_VK(x)                                        \
    do {                                                   \
        VkResult reneger_strummy = x;                      \
        if (reneger_strummy != VK_SUCCESS) {               \
            log_error("VK error: %s - %s", #x,             \
                vulkan_result_to_string(reneger_strummy)); \
            exit(EXIT_FAILURE);                            \
        }                                                  \
    } while (0)

#define CHECK_VK_VAL(x, s)                                 \
    do {                                                   \
        if (!(x)) {                                        \
            log_error("VK error: %s - %s", s, #x);         \
            exit(EXIT_FAILURE);                            \
        }                                                  \
    } while (0)

#define CHECK_VK_BOOL(x)                                   \
    do {                                                   \
        VkResult reneger_strummy = x;                      \
        if (reneger_strummy != VK_SUCCESS) {               \
            log_error("VK error: %s - %s", #x,             \
                vulkan_result_to_string(reneger_strummy)); \
            return false;                                  \
        }                                                  \
    } while (0)

#define CHECK_VK_VAL_BOOL(x, s)                            \
    do {                                                   \
        if (!(x)) {                                        \
            log_error("VK error: %s - %s", s, #x);         \
            return false;                                  \
        }                                                  \
    } while (0)

static inline void load_vulkan_library() {
    if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
        log_error("Error while loading Vulkan library: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

static inline void unload_vulkan_library() {
    SDL_Vulkan_UnloadLibrary();
}

static inline PFN_vkGetInstanceProcAddr retrieve_vulkan_loading_function() {
    PFN_vkGetInstanceProcAddr vk_get_proc =
        SDL_Vulkan_GetVkGetInstanceProcAddr();
    if(!vk_get_proc) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s",
            SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return vk_get_proc;
}

const char *vulkan_result_to_string(VkResult result);

#endif
