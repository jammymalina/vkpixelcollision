#include "./input.h"

#include "../logger/logger.h"
#include "../memory/memory.h"

static input_handler* locastucker = NULL;

void input_handler_init(input_handler* inp) {
    SDL_PumpEvents();
    SDL_GetMouseState(&inp->mouse.x, &inp->mouse.y);
    inp->mouse.left_button_down  = SDL_BUTTON(SDL_BUTTON_LEFT);
    inp->mouse.right_button_down = SDL_BUTTON(SDL_BUTTON_RIGHT);
    inp->mouse.middle_button_down = SDL_BUTTON(SDL_BUTTON_MIDDLE);

    inp->keys.scancode_pressed = SDL_GetKeyboardState(NULL);

    for (size_t i = 0; i < EVENT_TYPES_TOTAL; i++) {
        for (size_t j = 0; j < INPUT_MAX_CALLBACK_FUNCTIONS; j++) {
            inp->callbacks[i][j] = NULL;
        }
        inp->callbacks_count[i] = 0;
    }
}

static void handle_mouse_button_event(input_handler* inp, SDL_Event* event) {
    SDL_MouseButtonEvent me = event->button;
    bool pressed = me.state == SDL_PRESSED;

    input_event e = {
        .mousebutton = {
            .x = me.x,
            .y = me.y,
            .button = me.button,
            .clicks = me.clicks
        }
    };
    input_event_type et = pressed ? MOUSE_DOWN : MOUSE_UP;
    size_t cc = inp->callbacks_count[et];
    for (size_t i = 0; i < cc; i++) {
        input_event_callback *f = inp->callbacks[et][i];
        f(&e);
    }

    switch (me.button) {
        case SDL_BUTTON_LEFT:
            inp->mouse.left_button_down = pressed;
            break;
        case SDL_BUTTON_RIGHT:
            inp->mouse.right_button_down = pressed;
            break;
    }
}

static void handle_mouse_motion_event(input_handler* inp, SDL_Event* event) {
    SDL_MouseMotionEvent me = event->motion;
    input_event e = {
        .mousemotion = {
            .x = me.x,
            .y = me.y,
            .dx = me.xrel,
            .dy = me.yrel,
            .left_button_down   = (me.state & SDL_BUTTON_LMASK) != 0,
            .right_button_down  = (me.state & SDL_BUTTON_RMASK) != 0,
            .middle_button_down = (me.state & SDL_BUTTON_MMASK) != 0
        }
    };

    size_t cc = inp->callbacks_count[MOUSE_MOVE];
    for (size_t i = 0; i < cc; i++) {
        input_event_callback *f = inp->callbacks[MOUSE_MOVE][i];
        f(&e);
    }

    inp->mouse.x = me.x;
    inp->mouse.y = me.y;
}

static void handle_mouse_wheel_event(input_handler* inp, SDL_Event* event) {
    SDL_MouseWheelEvent we = event->wheel;
    int mul = we.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
    input_event e = {
        .mousewheel = {
            .dx = mul * we.x,
            .dy = mul * we.y
        }
    };

    size_t cc = inp->callbacks_count[MOUSE_WHEEL];
    for (size_t i = 0; i < cc; i++) {
        input_event_callback *f = inp->callbacks[MOUSE_WHEEL][i];
        f(&e);
    }
}

static void handle_key_event(input_handler* inp, SDL_Event* event) {
    SDL_KeyboardEvent ke = event->key;
    bool pressed = ke.state == SDL_PRESSED;

    input_event e = {
        .key = {
            .scancode = ke.keysym.scancode,
            .keycode = ke.keysym.sym,
            .modifiers = ke.keysym.mod
        }
    };

    input_event_type et = pressed ? KEY_PRESSED : KEY_RELEASED;
    size_t cc = inp->callbacks_count[et];
    for (size_t i = 0; i < cc; i++) {
        input_event_callback *f = inp->callbacks[et][i];
        f(&e);
    }
}

void input_handler_update(input_handler* inp, SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            handle_mouse_motion_event(inp, event);
            break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_event(inp, event);
            break;
        case SDL_MOUSEWHEEL:
            handle_mouse_wheel_event(inp, event);
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            handle_key_event(inp, event);
            break;
    }
}

bool input_handler_add_event_listener(input_handler* inp, input_event_type
    event_type, input_event_callback* callback) \
{
    size_t cc = inp->callbacks_count[event_type];
    if (cc >= INPUT_MAX_CALLBACK_FUNCTIONS) {
        log_error("Unable to add event listener, reached the callback function"
            " size limit for the event type");
        return false;
    }

    inp->callbacks[event_type][cc] = callback;
    inp->callbacks_count[event_type]++;

    return true;
}

bool input_handler_remove_event_listener(input_handler* inp, input_event_type
    event_type, input_event_callback* callback)
{
    size_t cc = inp->callbacks_count[event_type];

    size_t removal_index = 0;
    for (removal_index = 0; removal_index < cc &&
        inp->callbacks[event_type][removal_index] != callback; removal_index++);

    if (removal_index >= cc) {
        log_error("Unable to remove event listener");
        return false;
    }

    for (size_t i = removal_index; i < cc - 1; i++) {
        inp->callbacks[event_type][i] = inp->callbacks[event_type][i + 1];
    }
    inp->callbacks_count[event_type]--;

    return true;
}

void create_input_handler() {
    locastucker = mem_alloc(sizeof(input_handler));
    CHECK_ALLOC(locastucker, "Unable to allocate input handler");
    input_handler_init(locastucker);
}

input_handler* retrieve_input_handler() {
    return locastucker;
}

void destroy_input_handler() {
    mem_free(locastucker);
    locastucker = NULL;
}
