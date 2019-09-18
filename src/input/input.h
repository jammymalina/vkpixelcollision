
#ifndef PERIPHERAL_INPUT_H
#define PERIPHERAL_INPUT_H

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "./key_keycodes.h"
#include "./key_scancodes.h"

#define INPUT_MAX_CALLBACK_FUNCTIONS 32

#define MOUSE_LEFT_BUTTON   SDL_BUTTON_LEFT
#define MOUSE_RIGHT_BUTTON  SDL_BUTTON_RIGHT
#define MOUSE_MIDDLE_BUTTON SDL_BUTTON_MIDDLE

typedef enum input_event_type {
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE,
    MOUSE_WHEEL,
    KEY_PRESSED,
    KEY_RELEASED,
    EVENT_TYPES_TOTAL
} input_event_type;

typedef struct input_event {
    union {
        struct {
            int x, y;
            uint8_t button;
            uint8_t clicks;
        } mousebutton;

        struct {
            int x, y;
            int dx, dy;
            bool left_button_down, middle_button_down, right_button_down;
        } mousemotion;

        struct {
            int dx, dy;
        } mousewheel;

        struct {
            key_scancode scancode;
            key_keycode keycode;
            uint16_t modifiers;
        } key;
    };
} input_event;

typedef void input_event_callback(const input_event*);

typedef struct input_handler {
    struct {
        int x, y;
        bool left_button_down, middle_button_down, right_button_down;
    } mouse;
    struct {
        const uint8_t* scancode_pressed;
    } keys;
    input_event_callback* callbacks[EVENT_TYPES_TOTAL]
        [INPUT_MAX_CALLBACK_FUNCTIONS];
    size_t callbacks_count[EVENT_TYPES_TOTAL];
} input_handler;

void input_handler_init(input_handler* inp);
void input_handler_update(input_handler* inp, SDL_Event* event);
bool input_handler_add_event_listener(input_handler* inp, input_event_type
    event_type, input_event_callback* callback);
bool input_handler_remove_event_listener(input_handler* inp, input_event_type
    event_type, input_event_callback* callback);

void create_input_handler();
input_handler* retrieve_input_handler();
void destroy_input_handler();

#endif
