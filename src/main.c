#include "./app/vk_app.h"
#include "./logger/logger.h"
#include "./vulkan/memory/allocator/vma.h"

#define MS_PER_UPDATE 16

int main(int argc, char* args[]) {
    vk_app app;

    const shader_preload_item preloaded_shaders[] = {
        { .filepath = "shaders/basic/basic2d.frag.svm", .name = "basic2dfrag" },
        { .filepath = "shaders/basic/basic2d.vert.svm", .name = "basic2dvert" }
    };

    const shader_manager_program_create_info preloaded_shader_programs[] = {
        {
            .name = "basic2d", .gpu = NULL, .shaders_size = 2,
            .shaders = (const char *[]) { "basic2dfrag", "basic2dvert" },
            .vertex_layout = VERTEX_LAYOUT_POS_2_COL_4,
            .preconfigured_pipelines = (pipeline_create_info[]) {
                {
                    .state_bits = RST_BASIC_2D,
                    .render_pass = VK_NULL_HANDLE,
                    .store_in_program_cache = true
                }
            },
            .preconfigured_pipelines_size = 1
        }
    };

    vk_app_create_info app_info = {
        .name = "pixelcollision",
        .window_config = {
            .title = "Pixel collision",
            .width = 800,
            .height = 600,
            .fullscreen = false
        },
        .vma_allocator_config = {
            .gpu = NULL,
            .desired_device_local_memory_MB = 32,
            .desired_host_visible_memory_MB = 16,
            .min_blocks_size = 30,
            .min_garbage_size = 20,
            .number_of_frames = 0
        },
        .renderer_config = {
            .ctx = NULL,
            .clear_color = 0xF5F5F5FF,
            .clear_bits = CLEAR_COLOR_BUFFER
        },
        .render_programs_config = {
            .shd_tools_config = {
                .max_shader_program_byte_size = 32 * 1024, // 32 KB
                .expected_number_of_shaders = 32
            },
            .preloaded_shaders_config = {
                .preloaded_shaders = preloaded_shaders,
                .preloaded_shaders_size = sizeof(preloaded_shaders) /
                    sizeof(shader_preload_item)
            },
            .preloaded_shader_programs_config = {
                .default_gpu = NULL,
                .shader_programs_config = preloaded_shader_programs,
                .shader_programs_config_size = sizeof(preloaded_shader_programs)
                    / sizeof(shader_manager_program_create_info)
            }
        }
    };

    vk_app_init(&app, &app_info);

    log_info("Executable file path: %s", app.basepath);
    log_info("Window size: %d %d", app.window.width, app.window.height);
    log_info("Rendering context size: %d %d", app.ctx.width, app.ctx.height);
    log_info("Screen BPP: %d", SDL_BITSPERPIXEL(app.window.mode.format));
    log_info("Number of frames: %u", app.ctx.swapchain.image_count);

    bool is_running = true;

    uint32_t previous_time = SDL_GetTicks();
    double lag = 0.0;
    main_renderer* renderer = retrieve_main_renderer();
    while (is_running) {

        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed_time = current_time - previous_time;

        previous_time = current_time;
        lag += elapsed_time;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        is_running = false;
                    }
                    break;
            }
            // update_input(&event);
        }

        while (lag >= MS_PER_UPDATE) {
            // double delta = lag / MS_PER_UPDATE;
            lag -= MS_PER_UPDATE;
        }

        bool render_status = main_renderer_render(renderer);
        is_running &= render_status;
    }

    destroy_main_renderer();
    vk_app_destroy(&app);
}
