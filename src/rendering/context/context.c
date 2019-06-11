#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>

#include "../../logger/logger.h"
#include "../../memory/memory.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"

static inline VkSurfaceKHR retrieve_surface(VkInstance instance, SDL_Window* w)
{
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
    ctx->framebuffers = NULL;
    render_pass_init_empty(&ctx->render_pass);
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
    vk_swapchain_create_info sw_info = {
        .gpu = ctx->gpu,
        .dimensions = dimensions,
        .surface = ctx->surface
    };
    if (!swapchain_init(&ctx->swapchain, &sw_info)) {
        log_error("Unable to create swapchain");
        exit(EXIT_FAILURE);
    }
}

void rendering_context_init_render_pass(rendering_context *ctx) {
    if (!ctx->gpu) {
        log_info("Rendering context gpu pointer doesn't point to valid gpu");
        exit(EXIT_FAILURE);
    }

    if (!render_pass_init_swapchain_default_2d(&ctx->render_pass,
        ctx->gpu->device, ctx->swapchain.surface_format.format))
    {
        log_error("Unable to create render pass");
        exit(EXIT_FAILURE);
    }
}

void rendering_context_init_framebuffers(rendering_context* ctx) {
    if (!ctx->render_pass.handle || !ctx->gpu->device) {
        log_info("Unable to create framebuffers - render pass might not be "
            "initialized yet");
        exit(EXIT_FAILURE);
    }
    if (vk_framebuffer_init_from_swapchain(&ctx->framebuffers, &ctx->swapchain,
        ctx->render_pass.handle) != ctx->swapchain.image_count)
    {
        log_error("Unable to create framebuffers from swapchain");
        exit(EXIT_FAILURE);
    }
}

void rendering_context_destroy(rendering_context* ctx) {
    if (ctx->framebuffers) {
        for (size_t i = 0; i < ctx->swapchain.image_count; ++i) {
            vk_framebuffer_destroy(&ctx->framebuffers[i]);
        }
        mem_free(ctx->framebuffers);
        ctx->framebuffers = NULL;
    }
    render_pass_destroy(&ctx->render_pass);
    swapchain_destroy(&ctx->swapchain);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    rendering_context_init(ctx);
}
