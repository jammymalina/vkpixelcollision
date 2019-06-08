#include "./app_window/window.h"

#include <unistd.h>
#include "./logger/logger.h"
#include "./vulkan/memory/allocator/vma.h"
#include "./vulkan/memory/allocator/lib/vma_vector.h"

#define MS_PER_UPDATE 16

int main(int argc, char* args[]) {
    app_window_config window_config = {
        .title = "Collision app",
        .width = 800,
        .height = 600
    };
    app_window window = create_window(&window_config);
    rendering_context ctx = get_rendering_context(&window);

    log_info("Window size: %d %d", window.width, window.height);
    log_info("Rendering context size: %d %d", ctx.width, ctx.height);
    log_info("Screen BPP: %d", SDL_BITSPERPIXEL(window.mode.format));

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
            double delta = lag / MS_PER_UPDATE;
            lag -= MS_PER_UPDATE;
        }
    }

    destroy_vma_allocator();
    destroy_rendering_context(&ctx);
    destroy_window(&window);
}
