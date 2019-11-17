#include "./vk_app.h"

#include <stdlib.h>

#include "../input/input.h"
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

static inline void vk_app_init_gpu_queues(vk_app* app, const vk_app_create_info*
    app_info)
{
    gpu_queue_selector* queue_selector = retrieve_gpu_queue_selector();
    gpu_queue_select_response response;
    gpu_queue_select_query_status status;
    // we always have 1 present queue by default
    const gpu_queue_select_query query = {
        .usage = VK_QUEUE_GRAPHICS_BIT,
        .queue_group_name = "present",
        .support_present = true,
        .surface = app->ctx.surface,
        .queue_count = 1,
        .priority = 1.0
    };
    status = gpu_selector_select(queue_selector, &query, &response);
    ASSERT_LOG_ERROR_EXIT(status == GPU_SELECTOR_OK, "Unable to select present"
        " queue");
}

static inline void vk_app_init_gpu(vk_app* app, const vk_app_create_info
    *app_info)
{
    app->gpu = select_gpu(app->instance, app->ctx.surface);

    const gpu_queue_selector_create_info selector_info = {
        .gpu = &app->gpu
    };
    create_gpu_queue_selector(&selector_info);
    vk_app_init_gpu_queues(app, app_info);

    bool status = gpu_device_init(&app->gpu, app->ctx.surface);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to create logical device");
    load_device_level_functions(app->gpu.device);
    rendering_context_set_gpu(&app->ctx, &app->gpu);
}

static inline void vk_app_init_rendering_context(vk_app* app) {
    rendering_context_init(&app->ctx);
    rendering_context_set_instance(&app->ctx, app->instance);
    rendering_context_init_surface(&app->ctx, app->window.handle);
}

static inline void vk_app_init_rendering_context_graphics_queue(vk_app* app) {
    rendering_context_retrieve_graphics_queue(&app->ctx);
}

static inline void vk_app_init_rendering_context_swapchain(vk_app* app) {
    rendering_context_init_swapchain(&app->ctx);
}

static inline void vk_app_init_allocator(vk_app* app, const vk_app_create_info*
    app_info)
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

static inline void vk_app_init_renderer(vk_app* app, const vk_app_create_info*
    app_info)
{
    const size_t rf_size = app_info->renderer_config.render_functions_size;

    main_renderer_create_info renderer_config = {
        .ctx = app_info->renderer_config.ctx == NULL ? &app->ctx :
            app_info->renderer_config.ctx,
        .clear_color = app_info->renderer_config.clear_color,
        .clear_bits = app_info->renderer_config.clear_bits,
        .render_functions_size = rf_size
    };
    mem_copy(renderer_config.render_functions,
        app_info->renderer_config.render_functions, rf_size *
        sizeof(render_function));
    create_main_renderer(&renderer_config);
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

static inline void vk_app_init_multibuffer_manager(vk_app* app, const
    vk_app_create_info* app_info)
{
    create_vk_multibuffer_manager(&app_info->multibuffer_manager_config.config);

    vk_multibuffer_manager* mbm = retrieve_vk_multibuffer_manager();
    vk_multibuffer_manager_preload_info mbuff_info =
        app_info->multibuffer_manager_config.preloaded_multibuffers;
    mbuff_info.default_gpu = mbuff_info.default_gpu ?
        mbuff_info.default_gpu : &app->gpu;

    bool status = vk_multibuffer_manager_preload(mbm, &mbuff_info);
    ASSERT_LOG_ERROR_EXIT(status, "Unable to preload multibuffers");
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
    vk_app_init_gpu(app, app_info);
    vk_app_init_rendering_context_graphics_queue(app);
    vk_app_init_rendering_context_swapchain(app);
    vk_app_init_allocator(app, app_info);
    vk_app_init_rendering_context_render_pass(app);
    vk_app_init_renderer(app, app_info);
    vk_app_init_shaders(app, app_info);
    vk_app_init_multibuffer_manager(app, app_info);

    create_input_handler();
}

void vk_app_destroy(vk_app* app) {
    destroy_input_handler();

    vkDeviceWaitIdle(app->gpu.device);

    destroy_vk_multibuffer_manager();
    destroy_shader_program_manager();
    destroy_shader_manager();
    destroy_shader_loader();
    destroy_vma_allocator();

    destroy_main_renderer();
    rendering_context_destroy(&app->ctx);

    destroy_gpu_queue_selector();
    gpu_info_destroy(&app->gpu);
    destroy_vk_debugger();
    destroy_instance(app->instance);

    app->instance = VK_NULL_HANDLE;
    vk_app_window_destroy(&app->window);

    unload_vulkan_library();

}
