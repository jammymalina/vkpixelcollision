#ifndef COLLISION_APP_WINDOW_H
#define COLLISION_APP_WINDOW_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../rendering/context/context.h"

typedef struct app_window_config {
    const char *title;
    int width;
    int height;
    bool fullscreen;
} app_window_config;

typedef struct app_window {
    SDL_DisplayMode mode;
    SDL_Window *handle;
    int width, height;
} app_window;


app_window create_window(const app_window_config* config);
void destroy_window(app_window* window);

rendering_context get_rendering_context(const app_window* window);

#endif
