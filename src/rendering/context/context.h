#ifndef RENDERING_CONTEXT_H
#define RENDERING_CONTEXT_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "../renderer/rendering_resource.h"
#include "../../vulkan/framebuffer/framebuffer.h"
#include "../../vulkan/gpu/gpu.h"
#include "../../vulkan/queue/vk_queue.h"
#include "../../vulkan/render_pass/render_pass.h"
#include "../../vulkan/swapchain/swapchain.h"

typedef struct rendering_context {
    int width, height;
    VkInstance instance;
    gpu_info* gpu;

    VkSurfaceKHR surface;
    vk_swapchain swapchain;

    vk_queue graphics_queue;

    vk_render_pass render_pass;
    rendering_resource* rendering_resources;
} rendering_context;

void rendering_context_init(rendering_context* ctx);
void rendering_context_init_surface(rendering_context* ctx, SDL_Window* w);
void rendering_context_retrieve_graphics_queue(rendering_context* ctx);
void rendering_context_init_swapchain(rendering_context* ctx);
void rendering_context_init_render_pass(rendering_context *ctx);
void rendering_context_init_rendering_resources(rendering_context *ctx);
void rendering_context_destroy(rendering_context* ctx);

void rendering_context_set_instance(rendering_context* ctx, VkInstance inst);
void rendering_context_set_gpu(rendering_context* ctx, gpu_info *gpu);

#endif
