#ifndef COLLISION_APP_WINDOW_H
#define COLLISION_APP_WINDOW_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../../rendering/context/context.h"

typedef struct vk_app_window_create_info {
    char* title;
    int width;
    int height;
    bool fullscreen;
} vk_app_window_create_info;

typedef struct vk_app_window {
    SDL_DisplayMode mode;
    SDL_Window* handle;
    int width, height;
} vk_app_window;


void vk_app_window_init(vk_app_window* window, const vk_app_window_create_info*
    config);
void vk_app_window_destroy(vk_app_window* window);

#endif
