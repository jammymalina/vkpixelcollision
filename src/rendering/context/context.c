#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>

#include "../../logger/logger.h"
#include "../../memory/memory.h"
#include "../../vulkan/functions/functions.h"
#include "../../vulkan/gpu/gpu_queue_selector.h"
#include "../../vulkan/tools/tools.h"

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
    vk_queue_init_empty(&ctx->graphics_queue);
    ctx->width = 0;
    ctx->height = 0;
    ctx->instance = VK_NULL_HANDLE;
    ctx->surface = VK_NULL_HANDLE;
    vk_swapchain empty_swapchain = { 0 };
    ctx->swapchain = empty_swapchain;
    ctx->rendering_resources = NULL;
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

void rendering_context_retrieve_graphics_queue(rendering_context* ctx) {
    ASSERT_LOG_ERROR_EXIT(ctx->gpu, "Rendering context gpu pointer doesn't"
        " point to valid gpu");
    const gpu_queue_selector* queue_selector = retrieve_gpu_queue_selector();
    ASSERT_LOG_ERROR_EXIT(queue_selector, "GPU queue selector should be"
        " initialized");
    size_t qc = gpu_selector_retrieve_queue_group(queue_selector, "present",
        NULL);
    ASSERT_LOG_ERROR_EXIT(qc == 1, "Number of main present queues must be 1");
    gpu_selector_retrieve_queue_group(queue_selector, "present",
        &ctx->graphics_queue);
}

void rendering_context_init_swapchain(rendering_context* ctx) {
    ASSERT_LOG_ERROR_EXIT(ctx->gpu, "Rendering context gpu pointer doesn't"
        " point to valid gpu");
    const VkExtent2D dimensions = {
        .width = ctx->width,
        .height = ctx->height
    };
    const vk_swapchain_create_info sw_info = {
        .gpu = ctx->gpu,
        .dimensions = dimensions,
        .surface = ctx->surface
    };
    const bool status = swapchain_init(&ctx->swapchain, &sw_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create swapchain");
}

void rendering_context_init_render_pass(rendering_context *ctx) {
    ASSERT_LOG_ERROR_EXIT(ctx->gpu, "Rendering context gpu pointer doesn't"
        " point to valid gpu");
    const bool status = render_pass_init_swapchain_default_2d(&ctx->render_pass,
        ctx->gpu->device, ctx->swapchain.surface_format.format);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create render pass");
}

void rendering_context_init_rendering_resources(rendering_context* ctx) {
    ASSERT_LOG_ERROR_EXIT(ctx->render_pass.handle && ctx->gpu->device &&
        ctx->swapchain.handle && ctx->graphics_queue.handle, "Unable to create"
        " framebuffers - render pass might not be initialized yet");

    const bool status = rendering_resources_init_from_swapchain(
        &ctx->rendering_resources, ctx->gpu, &ctx->swapchain,
        ctx->render_pass.handle, ctx->graphics_queue.family_index);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create rendering resources");
}

void rendering_context_destroy(rendering_context* ctx) {
    if (ctx->rendering_resources) {
        for (size_t i = 0; i < ctx->swapchain.image_count; ++i) {
            rendering_resource_destroy(&ctx->rendering_resources[i]);
        }
        mem_free(ctx->rendering_resources);
        ctx->rendering_resources = NULL;
    }
    render_pass_destroy(&ctx->render_pass);
    swapchain_destroy(&ctx->swapchain);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    rendering_context_init(ctx);
}
