#include "./window.h"

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>

#include "../logger/logger.h"

static void init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        log_error("Error while initializing SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

app_window create_window(const app_window_config *config) {
    app_window w = {
        .handle = NULL
    };

    init_SDL();
    w.handle= config->fullscreen ? 
        SDL_CreateWindow(config->title, 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN) :
        SDL_CreateWindow(config->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config->width, config->height, 
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    if (!w.handle) {
        log_error("Unable to create SDL window");
        exit(EXIT_FAILURE);
    }
    SDL_GetWindowSize(w.handle, &w.width, &w.height);
    SDL_GetCurrentDisplayMode(0, &w.mode);

    return w;
}

void destroy_window(app_window *window) {
    SDL_DestroyWindow(window->handle);
    window->handle = NULL;
    SDL_Quit();
}

rendering_context get_rendering_context(const app_window *window) {
    rendering_context ctx = { 0 };

    init_rendering_context(&ctx, window->handle);

    return ctx;
}
