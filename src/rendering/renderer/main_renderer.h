#ifndef MAIN_RENDERER_H
#define MAIN_RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#include "../context/context.h"

enum clear_screen_bits {
    CLEAR_COLOR_BUFFER = 1,
    CLEAR_DEPTH_BUFFER = 2,
    CLEAR_STENCIL_BUFFER = 4
};

typedef struct main_renderer_create_info {
    const rendering_context* ctx;
    uint32_t clear_color;
    uint32_t clear_bits;
} main_renderer_create_info;

typedef struct main_renderer {
    uint32_t active_render_res_idx;
    const rendering_context* ctx;

    uint32_t clear_color;
    uint32_t clear_bits;
} main_renderer;

void main_renderer_init(main_renderer* renderer, const
    main_renderer_create_info* renderer_info);

const rendering_resource* main_renderer_active_render_res(const main_renderer*
    renderer);

bool main_renderer_render(main_renderer* renderer);

void create_main_renderer(const main_renderer_create_info* renderer_info);
main_renderer* retrieve_main_renderer();
void destroy_main_renderer();

#endif
