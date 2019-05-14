#ifndef RENDERING_CONTEXT_H
#define RENDERING_CONTEXT_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "../../vulkan/gpu/gpu.h"
#include "../../vulkan/swapchain/swapchain.h"

typedef struct rendering_context {
    int width, height;
    VkInstance instance;
    VkSurfaceKHR surface;

    gpu_info gpu;

    VkQueue graphics_queue;

    vk_swapchain swapchain;

    #ifdef DEBUG
        VkDebugReportCallbackEXT debug_callback;
    #endif
} rendering_context;

void init_rendering_context(rendering_context *ctx, SDL_Window *w);
void destroy_rendering_context(rendering_context *ctx);

#endif
