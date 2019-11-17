#ifndef MAIN_RENDERER_H
#define MAIN_RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#include "../context/context.h"
#include "./render_function.h"

#define FPS_60_DURATION_MS 16
#define FPS_30_DURATION_MS 33

#define MAIN_RENDERING_MAX_RENDER_FUNCTIONS_SIZE 32

enum clear_screen_bits {
    CLEAR_COLOR_BUFFER = 1,
    CLEAR_DEPTH_BUFFER = 2,
    CLEAR_STENCIL_BUFFER = 4
};
typedef struct main_renderer_create_info {
    const rendering_context* ctx;
    uint32_t clear_color;
    uint32_t clear_bits;
    render_function render_functions[MAIN_RENDERING_MAX_RENDER_FUNCTIONS_SIZE];
    size_t render_functions_size;
} main_renderer_create_info;

typedef struct main_renderer {
    uint32_t active_render_res_idx;
    const rendering_context* ctx;

    uint32_t clear_color;
    uint32_t clear_bits;

    render_function render_functions[MAIN_RENDERING_MAX_RENDER_FUNCTIONS_SIZE];
    size_t render_functions_size;
} main_renderer;

void main_renderer_init(main_renderer* renderer, const
    main_renderer_create_info* renderer_info);
void main_renderer_set_render_functions(main_renderer* renderer, const
    render_function render_functions[MAIN_RENDERING_MAX_RENDER_FUNCTIONS_SIZE],
    size_t render_functions_size);
const rendering_resource* main_renderer_active_render_res(const main_renderer*
    renderer);
bool main_renderer_render(main_renderer* renderer);

void create_main_renderer(const main_renderer_create_info* renderer_info);
main_renderer* retrieve_main_renderer();
void destroy_main_renderer();

#endif
