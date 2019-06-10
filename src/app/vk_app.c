#include "./vk_app.h"

#include <stdlib.h>
#include <unistd.h>

#include "../string/string.h"
#include "../vulkan/debug/debug.h"
#include "../vulkan/functions/function_loader.h"
#include "../vulkan/functions/functions.h"
#include "../vulkan/gpu/gpu_selector.h"
#include "../vulkan/instance/instance.h"
#include "../vulkan/tools/tools.h"

static inline bool vk_app_set_exe_filepath(vk_app* app) {
    return path_retrieve_executable_filepath(app->exe_filepath) &&
        path_retrieve_executable_dir(app->exe_directory);
}

static inline void vk_app_init_vulkan(vk_app* app) {
    load_vulkan_library();
    load_external_function(retrieve_vulkan_loading_function());
    load_global_functions();
    app->instance = create_instance(app->window.handle);
    load_instance_vulkan_functions(app->instance);
    init_vk_debugger(app->instance);
}

static inline void vk_app_init_gpu(vk_app* app) {
    app->gpu = select_gpu(app->instance, app->ctx.surface);
    gpu_device_init(&app->gpu, app->ctx.surface);
    load_device_level_functions(app->gpu.device);
    rendering_context_set_gpu(&app->ctx, &app->gpu);
}

static inline void vk_app_init_rendering_context(vk_app* app) {
    rendering_context_init(&app->ctx);
    rendering_context_set_instance(&app->ctx, app->instance);
    rendering_context_init_surface(&app->ctx, app->window.handle);
}

static inline void vk_app_init_rendering_context_swapchain(vk_app* app) {
    rendering_context_init_swapchain(&app->ctx);
}

static inline void vk_app_create_window(vk_app* app, const vk_app_create_info
    *app_info)
{
    string_copy(app->name, APP_NAME_MAX_SIZE, app_info->name);
    vk_app_window_init(&app->window, &app_info->window_config);
}

void vk_app_init(vk_app* app, const vk_app_create_info* app_info) {
    if (!vk_app_set_exe_filepath(app)) {
        exit(EXIT_FAILURE);
    }
    vk_app_create_window(app, app_info);
    vk_app_init_vulkan(app);
    vk_app_init_rendering_context(app);
    vk_app_init_gpu(app);
    vk_app_init_rendering_context_swapchain(app);
}

void vk_app_destroy(vk_app* app) {
    vkDeviceWaitIdle(app->gpu.device);

    rendering_context_destroy(&app->ctx);

    destroy_gpu_info(&app->gpu);
    destroy_vk_debugger();
    destroy_instance(app->instance);
    app->instance = VK_NULL_HANDLE;
    vk_app_window_destroy(&app->window);

    unload_vulkan_library();
}
