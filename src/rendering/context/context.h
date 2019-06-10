#ifndef RENDERING_CONTEXT_H
#define RENDERING_CONTEXT_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "../../vulkan/gpu/gpu.h"
#include "../../vulkan/render_pass/render_pass.h"
#include "../../vulkan/swapchain/swapchain.h"

typedef struct rendering_context {
    int width, height;
    VkInstance instance;
    gpu_info* gpu;

    VkSurfaceKHR surface;
    vk_swapchain swapchain;
    VkImage* swapchain_images;

    VkQueue graphics_queue;

    vk_render_pass render_pass;
} rendering_context;

void rendering_context_init(rendering_context* ctx);
void rendering_context_init_surface(rendering_context* ctx, SDL_Window* w);
void rendering_context_init_device_queue(rendering_context* ctx);
void rendering_context_init_swapchain(rendering_context* ctx);
void rendering_context_init_render_pass(rendering_context *ctx);
void rendering_context_destroy(rendering_context* ctx);

void rendering_context_set_instance(rendering_context* ctx, VkInstance inst);
void rendering_context_set_gpu(rendering_context* ctx, gpu_info *gpu);

#endif
