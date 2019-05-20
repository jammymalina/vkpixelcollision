#include "./context.h"

#include <SDL2/SDL_vulkan.h>

#include "../../logger/logger.h"
#include "../../vulkan/debug/debug.h"
#include "../../vulkan/functions/function_loader.h"
#include "../../vulkan/functions/functions.h"
#include "../../vulkan/gpu/gpu_selector.h"
#include "../../vulkan/instance/instance.h"

static void load_vulkan_library() {
    if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
        log_error("Error while loading Vulkan library: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

static PFN_vkGetInstanceProcAddr retrieve_vulkan_loading_function() {
    PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
    if(!vk_get_proc) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return vk_get_proc;
}

static VkSurfaceKHR retrieve_surface(VkInstance instance, SDL_Window* w) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(w, instance, &surface)) {
        log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return surface;
}

void init_rendering_context(rendering_context* ctx, SDL_Window* w) {
    load_vulkan_library();
    SDL_Vulkan_GetDrawableSize(w, &ctx->width, &ctx->height);
    load_external_function(retrieve_vulkan_loading_function());
    load_global_functions();
    ctx->instance = create_instance(w);
    load_instance_vulkan_functions(ctx->instance);
    #ifdef DEBUG
        ctx->debug_callback = setup_debug_callback(ctx->instance, DEFAULT_VULKAN_DEBUG_FLAFS);
    #endif
    ctx->surface = retrieve_surface(ctx->instance, w);
    ctx->gpu = select_gpu(ctx->instance, ctx->surface);
    init_gpu_device(&ctx->gpu, ctx->surface);  
    load_device_level_functions(ctx->gpu.device);
    vkGetDeviceQueue(ctx->gpu.device, retrieve_graphics_queue_index(&ctx->gpu, ctx->surface), 0, 
        &ctx->graphics_queue);
    VkExtent2D dimensions = { .width = ctx->width, .height = ctx->height };
    ctx->swapchain = create_swapchain(&ctx->gpu, ctx->surface, &dimensions);
}

void destroy_rendering_context(rendering_context* ctx) {
    vkDeviceWaitIdle(ctx->gpu.device);

    destroy_swapchain(&ctx->swapchain, ctx->gpu.device);
    destroy_gpu_info(&ctx->gpu);
    
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    ctx->surface = VK_NULL_HANDLE;

    #ifdef DEBUG
        vkDestroyDebugReportCallbackEXT(ctx->instance, ctx->debug_callback, NULL);
    #endif

    destroy_instance(ctx->instance);
    ctx->instance = VK_NULL_HANDLE;

    SDL_Vulkan_UnloadLibrary();
}