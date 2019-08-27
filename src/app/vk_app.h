#ifndef VULKAN_APP_SKELETON_H
#define VULKAN_APP_SKELETON_H

#include "../file/path.h"
#include "../rendering/renderer/main_renderer.h"
#include "../vulkan/gpu/gpu.h"
#include "../vulkan/memory/allocator/vma.h"
#include "../vulkan/shader/shd.h"
#include "./window/window.h"

#define APP_NAME_MAX_SIZE 256

typedef struct vk_app_create_info {
    char* name;

    vk_app_window_create_info window_config;
    vma_allocator_create_info vma_allocator_config;
    main_renderer_create_info renderer_config;
    struct {
        shd_tools_create_info shd_tools_config;
        shader_preload_info preloaded_shaders_config;
        shader_program_preload_info preloaded_shader_programs_config;
    } render_programs_config;
} vk_app_create_info;

typedef struct vk_app {
    char name[APP_NAME_MAX_SIZE];
    char basepath[PATH_MAX_SIZE];

    vk_app_window window;
    rendering_context ctx;

    VkInstance instance;
    gpu_info gpu;
} vk_app;

void vk_app_init(vk_app* app, const vk_app_create_info* app_info);
void vk_app_destroy(vk_app* app);

#endif
