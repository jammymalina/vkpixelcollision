#include "./vk_app.h"

#include <stdlib.h>

#include "../string/string.h"
#include "../vulkan/debug/debug.h"
#include "../vulkan/functions/function_loader.h"
#include "../vulkan/functions/functions.h"
#include "../vulkan/gpu/gpu_queue_selector.h"
#include "../vulkan/gpu/gpu_selector.h"
#include "../vulkan/instance/instance.h"
#include "../vulkan/tools/tools.h"

static inline void vk_app_set_basepath(vk_app* app) {
    if (!path_retrieve_basepath(app->basepath)) {
        exit(EXIT_FAILURE);
    }
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
    bool status = gpu_device_init(&app->gpu, app->ctx.surface);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create logical device");
    load_device_level_functions(app->gpu.device);
    const gpu_queue_selector_create_info selector_info = {
        .gpu = &app->gpu
    };
    create_gpu_queue_selector(&selector_info);
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

static inline void vk_app_init_allocator(vk_app* app, const vk_app_create_info
    *app_info)
{
    vma_allocator_create_info allocator_info = app_info->vma_allocator_config;
    if (!allocator_info.gpu) {
        allocator_info.gpu = &app->gpu;
    }
    if (allocator_info.number_of_frames == 0) {
        allocator_info.number_of_frames = app->ctx.swapchain.image_count;
    }
    create_vma_allocator(&allocator_info);
}

static inline void vk_app_init_rendering_context_render_pass(vk_app* app) {
    rendering_context_init_render_pass(&app->ctx);
    rendering_context_init_rendering_resources(&app->ctx);
}

static inline void vk_app_init_shaders(vk_app* app,
    const vk_app_create_info *app_info)
{
    const shd_tools_create_info* tools_config =
        &app_info->render_programs_config.shd_tools_config;
    create_shader_loader(tools_config);
    create_shader_manager(tools_config);
    create_shader_program_manager(tools_config);

    shader_manager* shm = retrieve_shader_manager();
    bool status = shader_manager_preload(shm,
        &app_info->render_programs_config.preloaded_shaders_config,
        app->basepath, &app->gpu);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to load all shaders");

    shader_program_manager* spm = retrieve_shader_program_manager();
    shader_program_preload_info sh_info =
        app_info->render_programs_config.preloaded_shader_programs_config;
    sh_info.default_gpu = sh_info.default_gpu ?
        sh_info.default_gpu : &app->gpu;
    sh_info.default_render_pass = sh_info.default_render_pass ?
        sh_info.default_render_pass : app->ctx.render_pass.handle;
    status = shader_program_manager_preload(spm, &sh_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to build all shader programs");
}

static inline void vk_app_create_window(vk_app* app, const vk_app_create_info
    *app_info)
{
    string_copy(app->name, APP_NAME_MAX_SIZE, app_info->name);
    vk_app_window_init(&app->window, &app_info->window_config);
}

void vk_app_init(vk_app* app, const vk_app_create_info* app_info) {
    vk_app_set_basepath(app);
    vk_app_create_window(app, app_info);
    vk_app_init_vulkan(app);
    vk_app_init_rendering_context(app);
    vk_app_init_gpu(app);
    vk_app_init_rendering_context_swapchain(app);
    vk_app_init_allocator(app, app_info);
    vk_app_init_rendering_context_render_pass(app);
    vk_app_init_shaders(app, app_info);
}

void vk_app_destroy(vk_app* app) {
    vkDeviceWaitIdle(app->gpu.device);

    destroy_shader_program_manager();
    destroy_shader_manager();
    destroy_shader_loader();
    destroy_vma_allocator();

    rendering_context_destroy(&app->ctx);

    destroy_gpu_queue_selector();
    gpu_info_destroy(&app->gpu);
    destroy_vk_debugger();
    destroy_instance(app->instance);
    app->instance = VK_NULL_HANDLE;
    vk_app_window_destroy(&app->window);

    unload_vulkan_library();
}
