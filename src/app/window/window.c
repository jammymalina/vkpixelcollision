#include "./window.h"

#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>

#include "../../logger/logger.h"

static void init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        log_error("Error while initializing SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void vk_app_window_init(vk_app_window* window, const vk_app_window_create_info*
    config)
{
    window->handle = NULL;

    init_SDL();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window->handle= config->fullscreen ?
        SDL_CreateWindow(config->title, 0, 0, display_mode.w, display_mode.h,
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN) :
        SDL_CreateWindow(config->title, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, config->width, config->height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

    if (!window->handle) {
        log_error("Unable to create SDL window");
        exit(EXIT_FAILURE);
    }

    SDL_GetWindowSize(window->handle, &window->width, &window->height);
    SDL_GetCurrentDisplayMode(0, &window->mode);
}

void vk_app_window_destroy(vk_app_window* window) {
    SDL_DestroyWindow(window->handle);
    window->handle = NULL;
    SDL_Quit();
}
