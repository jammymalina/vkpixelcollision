#include "./app/vk_app.h"
#include "./logger/logger.h"
#include "./vulkan/memory/allocator/vma.h"

#include "./collections/hash_string_map.h"

#define MS_PER_UPDATE 16

int main(int argc, char* args[]) {
    // typedef struct hash_boom_map HASH_STRING_MAP(int) hash_boom_map;

    // hash_boom_map map;
    // hash_string_map_init(&map, -1);
    // bool status = hash_string_map_reserve(&map, 1000);
    // log_info("%ld %ld", hash_string_map_get_capacity(&map),
    //     hash_string_map_get_size(&map));
    // hash_string_map_clear(&map);

    vk_app app;

    vk_app_create_info app_info = {
        .name = "pixelcollision",
        .window_config = {
            .title = "Pixel collision",
            .width = 800,
            .height = 600,
            .fullscreen = false
        },
        .vma_allocator_config = {
            .desired_device_local_memory_MB = 32,
            .desired_host_visible_memory_MB = 16,
            .min_blocks_size = 30,
            .min_garbage_size = 20,
            .number_of_frames = 0,
            .buffer_image_granularity = 0
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
    }

    vk_app_destroy(&app);
}
