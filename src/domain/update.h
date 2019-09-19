#ifndef DOMAIN_UPDATE_H
#define DOMAIN_UPDATE_H

#include <stdbool.h>

#include "../input/input.h"
#include "../logger/logger.h"

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

    return true;
}

static void domain_update(double delta) {
}

#endif
