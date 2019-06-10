#include "./vk_app.h"

#include <stdlib.h>
#include <unistd.h>

#include "../string/string.h"

static inline bool vk_app_set_exe_filepath(vk_app* app) {
    return path_retrieve_executable_filepath(app->exe_filepath) &&
        path_retrieve_executable_dir(app->exe_directory);
}

static inline void vk_app_create_window(vk_app* app, const vk_app_create_info
    *app_info)
{
    string_copy(app->name, APP_NAME_MAX_SIZE, app_info->name);
    vk_app_window_init(&app->window, &app_info->window_config);
    app->ctx = get_rendering_context(&app->window);
}

void vk_app_init(vk_app* app, const vk_app_create_info* app_info) {
    if (!vk_app_set_exe_filepath(app)) {
        exit(EXIT_FAILURE);
    }
    vk_app_create_window(app, app_info);
}

void vk_app_destroy(vk_app* app) {
    destroy_rendering_context(&app->ctx);
    vk_app_window_destroy(&app->window);
}
