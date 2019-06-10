#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>

#include "../../logger/logger.h"
#include "../../vulkan/functions/functions.h"

static inline VkSurfaceKHR retrieve_surface(VkInstance instance, SDL_Window* w) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(w, instance, &surface)) {
        log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return surface;
}

void rendering_context_init(rendering_context* ctx) {
    ctx->gpu = NULL;
    ctx->graphics_queue = VK_NULL_HANDLE;
    ctx->width = 0;
    ctx->height = 0;
    ctx->instance = VK_NULL_HANDLE;
    ctx->surface = VK_NULL_HANDLE;
    vk_swapchain empty_swapchain = { 0 };
    ctx->swapchain = empty_swapchain;
}

void rendering_context_init_surface(rendering_context* ctx, SDL_Window* w) {
    if (!ctx->instance) {
        log_error("Rendering pointer instance is not set");
        exit(EXIT_FAILURE);
    }
    SDL_Vulkan_GetDrawableSize(w, &ctx->width, &ctx->height);
    ctx->surface = retrieve_surface(ctx->instance, w);
}

void rendering_context_set_instance(rendering_context* ctx, VkInstance inst) {
    ctx->instance = inst;
}

void rendering_context_set_gpu(rendering_context* ctx, gpu_info *gpu) {
    ctx->gpu = gpu;
}

void rendering_context_init_device_queue(rendering_context* ctx) {
    if (!ctx->gpu) {
        log_error("Rendering context gpu pointer doesn't point to valid gpu");
        exit(EXIT_FAILURE);
    }
    vkGetDeviceQueue(ctx->gpu->device, retrieve_graphics_queue_index(ctx->gpu,
        ctx->surface), 0,
        &ctx->graphics_queue);
}

void rendering_context_init_swapchain(rendering_context* ctx) {
    if (!ctx->gpu) {
        log_info("Rendering context gpu pointer doesn't point to valid gpu");
        exit(EXIT_FAILURE);
    }
    VkExtent2D dimensions = { .width = ctx->width, .height = ctx->height };
    ctx->swapchain = create_swapchain(ctx->gpu, ctx->surface, &dimensions);
}

void rendering_context_destroy(rendering_context* ctx) {
    destroy_swapchain(&ctx->swapchain, ctx->gpu->device);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    rendering_context_init(ctx);
}
