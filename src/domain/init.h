#ifndef DOMAIN_INIT_H
#define DOMAIN_INIT_H

#include <stdbool.h>

#include "../input/input.h"
#include "../logger/logger.h"
#include "../math/mat4.h"
#include "../material/color.h"
#include "../vulkan/buffer/multibuffer.h"
#include "../vulkan/buffer/multibuffer_manager.h"

static void domain_mouse_down(const input_event* event) {
}

static void domain_mouse_move(const input_event* event) {
}

static bool domain_init() {
    input_handler* input = retrieve_input_handler();
    ASSERT_LOG_ERROR(input_handler_add_event_listener(input, MOUSE_DOWN,
        domain_mouse_down), "Unable to add mouse down event listener");
    ASSERT_LOG_ERROR(input_handler_add_event_listener(input, MOUSE_MOVE,
        domain_mouse_move), "Unable to add mouse move event listener");

    vk_multibuffer_manager* buff_manager = retrieve_vk_multibuffer_manager();
    vk_multibuffer* buff = vk_multibuffer_manager_get_reference(buff_manager,
        "basic");
    ASSERT_LOG_ERROR(buff, "Unable to retrive basic multibuffer");

    mat4 ortho_mat;
    ortho4(ortho_mat, 0, 1200.0, 800.0, 0);

    vk_multibuffer_segment_start_transaction(buff, "uniform_segment");
    vk_multibuffer_segment_append(buff, "uniform_segment", ortho_mat,
        sizeof(mat4));
    vk_multibuffer_segment_end_transaction(buff, "uniform_segment");

    float x = 0.0, y = 0.0;
    float line_color[4] = { 0.0, 0.0, 0.0, 0.0 };
    color_rgba_uint32_to_float(line_color, 0x008897);

    vk_multibuffer_segment_start_transaction(buff, "vertex_segment");
    for (x = 40.0; x < 1200.0; x += 40.0) {
        y = 0.0;
        vk_multibuffer_segment_append(buff, "vertex_segment", &x,
            sizeof(float));
        vk_multibuffer_segment_append(buff, "vertex_segment", &y,
            sizeof(float));
        y = 800.0;
        vk_multibuffer_segment_append(buff, "vertex_segment", line_color,
            sizeof(line_color));
        vk_multibuffer_segment_append(buff, "vertex_segment", &x,
            sizeof(float));
        vk_multibuffer_segment_append(buff, "vertex_segment", &y,
            sizeof(float));
        vk_multibuffer_segment_append(buff, "vertex_segment", line_color,
            sizeof(line_color));
    }
    vk_multibuffer_segment_end_transaction(buff, "vertex_segment");

    return true;
}

#endif
