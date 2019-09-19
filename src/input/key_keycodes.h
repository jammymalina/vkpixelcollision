#ifndef KEY_KEYCODE_H
#define KEY_KEYCODE_H

#include <SDL2/SDL.h>

#include "./key_scancodes.h"

#define KEY_SCANCODE_TO_KEYCODE SDL_SCANCODE_TO_KEYCODE

typedef enum key_keycode {
    KEY_KEYCODE_UNKNOWN = 0,

    KEY_KEYCODE_RETURN = '\r',
    KEY_KEYCODE_ESCAPE = '\033',
    KEY_KEYCODE_BACKSPACE = '\b',
    KEY_KEYCODE_TAB = '\t',
    KEY_KEYCODE_SPACE = ' ',
    KEY_KEYCODE_EXCLAIM = '!',
    KEY_KEYCODE_QUOTEDBL = '"',
    KEY_KEYCODE_HASH = '#',
    KEY_KEYCODE_PERCENT = '%',
    KEY_KEYCODE_DOLLAR = '$',
    KEY_KEYCODE_AMPERSAND = '&',
    KEY_KEYCODE_QUOTE = '\'',
    KEY_KEYCODE_LEFTPAREN = '(',
    KEY_KEYCODE_RIGHTPAREN = ')',
    KEY_KEYCODE_ASTERISK = '*',
    KEY_KEYCODE_PLUS = '+',
    KEY_KEYCODE_COMMA = ',',
    KEY_KEYCODE_MINUS = '-',
    KEY_KEYCODE_PERIOD = '.',
    KEY_KEYCODE_SLASH = '/',
    KEY_KEYCODE_0 = '0',
    KEY_KEYCODE_1 = '1',
    KEY_KEYCODE_2 = '2',
    KEY_KEYCODE_3 = '3',
    KEY_KEYCODE_4 = '4',
    KEY_KEYCODE_5 = '5',
    KEY_KEYCODE_6 = '6',
    KEY_KEYCODE_7 = '7',
    KEY_KEYCODE_8 = '8',
    KEY_KEYCODE_9 = '9',
    KEY_KEYCODE_COLON = ':',
    KEY_KEYCODE_SEMICOLON = ';',
    KEY_KEYCODE_LESS = '<',
    KEY_KEYCODE_EQUALS = '=',
    KEY_KEYCODE_GREATER = '>',
    KEY_KEYCODE_QUESTION = '?',
    KEY_KEYCODE_AT = '@',

    KEY_KEYCODE_LEFTBRACKET = '[',
    KEY_KEYCODE_BACKSLASH = '\\',
    KEY_KEYCODE_RIGHTBRACKET = ']',
    KEY_KEYCODE_CARET = '^',
    KEY_KEYCODE_UNDERSCORE = '_',
    KEY_KEYCODE_BACKQUOTE = '`',
    KEY_KEYCODE_a = 'a',
    KEY_KEYCODE_b = 'b',
    KEY_KEYCODE_c = 'c',
    KEY_KEYCODE_d = 'd',
    KEY_KEYCODE_e = 'e',
    KEY_KEYCODE_f = 'f',
    KEY_KEYCODE_g = 'g',
    KEY_KEYCODE_h = 'h',
    KEY_KEYCODE_i = 'i',
    KEY_KEYCODE_j = 'j',
    KEY_KEYCODE_k = 'k',
    KEY_KEYCODE_l = 'l',
    KEY_KEYCODE_m = 'm',
    KEY_KEYCODE_n = 'n',
    KEY_KEYCODE_o = 'o',
    KEY_KEYCODE_p = 'p',
    KEY_KEYCODE_q = 'q',
    KEY_KEYCODE_r = 'r',
    KEY_KEYCODE_s = 's',
    KEY_KEYCODE_t = 't',
    KEY_KEYCODE_u = 'u',
    KEY_KEYCODE_v = 'v',
    KEY_KEYCODE_w = 'w',
    KEY_KEYCODE_x = 'x',
    KEY_KEYCODE_y = 'y',
    KEY_KEYCODE_z = 'z',

    KEY_KEYCODE_CAPSLOCK = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CAPSLOCK),

    KEY_KEYCODE_F1 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F1),
    KEY_KEYCODE_F2 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F2),
    KEY_KEYCODE_F3 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F3),
    KEY_KEYCODE_F4 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F4),
    KEY_KEYCODE_F5 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F5),
    KEY_KEYCODE_F6 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F6),
    KEY_KEYCODE_F7 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F7),
    KEY_KEYCODE_F8 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F8),
    KEY_KEYCODE_F9 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F9),
    KEY_KEYCODE_F10 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F10),
    KEY_KEYCODE_F11 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F11),
    KEY_KEYCODE_F12 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F12),

    KEY_KEYCODE_PRINTSCREEN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PRINTSCREEN),
    KEY_KEYCODE_SCROLLLOCK = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_SCROLLLOCK),
    KEY_KEYCODE_PAUSE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PAUSE),
    KEY_KEYCODE_INSERT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_INSERT),
    KEY_KEYCODE_HOME = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_HOME),
    KEY_KEYCODE_PAGEUP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PAGEUP),
    KEY_KEYCODE_DELETE = '\177',
    KEY_KEYCODE_END = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_END),
    KEY_KEYCODE_PAGEDOWN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PAGEDOWN),
    KEY_KEYCODE_RIGHT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RIGHT),
    KEY_KEYCODE_LEFT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_LEFT),
    KEY_KEYCODE_DOWN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_DOWN),
    KEY_KEYCODE_UP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_UP),

    KEY_KEYCODE_NUMLOCKCLEAR = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_NUMLOCKCLEAR),
    KEY_KEYCODE_KP_DIVIDE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_DIVIDE),
    KEY_KEYCODE_KP_MULTIPLY = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_MULTIPLY),
    KEY_KEYCODE_KP_MINUS = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_MINUS),
    KEY_KEYCODE_KP_PLUS = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_PLUS),
    KEY_KEYCODE_KP_ENTER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_ENTER),
    KEY_KEYCODE_KP_1 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_1),
    KEY_KEYCODE_KP_2 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_2),
    KEY_KEYCODE_KP_3 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_3),
    KEY_KEYCODE_KP_4 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_4),
    KEY_KEYCODE_KP_5 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_5),
    KEY_KEYCODE_KP_6 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_6),
    KEY_KEYCODE_KP_7 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_7),
    KEY_KEYCODE_KP_8 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_8),
    KEY_KEYCODE_KP_9 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_9),
    KEY_KEYCODE_KP_0 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_0),
    KEY_KEYCODE_KP_PERIOD = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_PERIOD),

    KEY_KEYCODE_APPLICATION = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_APPLICATION),
    KEY_KEYCODE_POWER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_POWER),
    KEY_KEYCODE_KP_EQUALS = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_EQUALS),
    KEY_KEYCODE_F13 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F13),
    KEY_KEYCODE_F14 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F14),
    KEY_KEYCODE_F15 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F15),
    KEY_KEYCODE_F16 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F16),
    KEY_KEYCODE_F17 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F17),
    KEY_KEYCODE_F18 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F18),
    KEY_KEYCODE_F19 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F19),
    KEY_KEYCODE_F20 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F20),
    KEY_KEYCODE_F21 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F21),
    KEY_KEYCODE_F22 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F22),
    KEY_KEYCODE_F23 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F23),
    KEY_KEYCODE_F24 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_F24),
    KEY_KEYCODE_EXECUTE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_EXECUTE),
    KEY_KEYCODE_HELP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_HELP),
    KEY_KEYCODE_MENU = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_MENU),
    KEY_KEYCODE_SELECT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_SELECT),
    KEY_KEYCODE_STOP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_STOP),
    KEY_KEYCODE_AGAIN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AGAIN),
    KEY_KEYCODE_UNDO = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_UNDO),
    KEY_KEYCODE_CUT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CUT),
    KEY_KEYCODE_COPY = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_COPY),
    KEY_KEYCODE_PASTE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PASTE),
    KEY_KEYCODE_FIND = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_FIND),
    KEY_KEYCODE_MUTE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_MUTE),
    KEY_KEYCODE_VOLUMEUP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_VOLUMEUP),
    KEY_KEYCODE_VOLUMEDOWN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_VOLUMEDOWN),
    KEY_KEYCODE_KP_COMMA = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_COMMA),
    KEY_KEYCODE_KP_EQUALSAS400 = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_EQUALSAS400),

    KEY_KEYCODE_ALTERASE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_ALTERASE),
    KEY_KEYCODE_SYSREQ = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_SYSREQ),
    KEY_KEYCODE_CANCEL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CANCEL),
    KEY_KEYCODE_CLEAR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CLEAR),
    KEY_KEYCODE_PRIOR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_PRIOR),
    KEY_KEYCODE_RETURN2 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RETURN2),
    KEY_KEYCODE_SEPARATOR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_SEPARATOR),
    KEY_KEYCODE_OUT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_OUT),
    KEY_KEYCODE_OPER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_OPER),
    KEY_KEYCODE_CLEARAGAIN = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CLEARAGAIN),
    KEY_KEYCODE_CRSEL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CRSEL),
    KEY_KEYCODE_EXSEL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_EXSEL),

    KEY_KEYCODE_KP_00 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_00),
    KEY_KEYCODE_KP_000 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_000),
    KEY_KEYCODE_THOUSANDSSEPARATOR = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_THOUSANDSSEPARATOR),
    KEY_KEYCODE_DECIMALSEPARATOR = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_DECIMALSEPARATOR),
    KEY_KEYCODE_CURRENCYUNIT = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_CURRENCYUNIT),
    KEY_KEYCODE_CURRENCYSUBUNIT = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_CURRENCYSUBUNIT),
    KEY_KEYCODE_KP_LEFTPAREN = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_LEFTPAREN),
    KEY_KEYCODE_KP_RIGHTPAREN = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_RIGHTPAREN),
    KEY_KEYCODE_KP_LEFTBRACE = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_LEFTBRACE),
    KEY_KEYCODE_KP_RIGHTBRACE = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_RIGHTBRACE),
    KEY_KEYCODE_KP_TAB = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_TAB),
    KEY_KEYCODE_KP_BACKSPACE = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_BACKSPACE),
    KEY_KEYCODE_KP_A = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_A),
    KEY_KEYCODE_KP_B = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_B),
    KEY_KEYCODE_KP_C = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_C),
    KEY_KEYCODE_KP_D = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_D),
    KEY_KEYCODE_KP_E = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_E),
    KEY_KEYCODE_KP_F = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_F),
    KEY_KEYCODE_KP_XOR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_XOR),
    KEY_KEYCODE_KP_POWER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_POWER),
    KEY_KEYCODE_KP_PERCENT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_PERCENT),
    KEY_KEYCODE_KP_LESS = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_LESS),
    KEY_KEYCODE_KP_GREATER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_GREATER),
    KEY_KEYCODE_KP_AMPERSAND = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_AMPERSAND),
    KEY_KEYCODE_KP_DBLAMPERSAND = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_DBLAMPERSAND),
    KEY_KEYCODE_KP_VERTICALBAR = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_VERTICALBAR),
    KEY_KEYCODE_KP_DBLVERTICALBAR = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_DBLVERTICALBAR),
    KEY_KEYCODE_KP_COLON = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_COLON),
    KEY_KEYCODE_KP_HASH = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_HASH),
    KEY_KEYCODE_KP_SPACE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_SPACE),
    KEY_KEYCODE_KP_AT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_AT),
    KEY_KEYCODE_KP_EXCLAM = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_EXCLAM),
    KEY_KEYCODE_KP_MEMSTORE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_MEMSTORE),
    KEY_KEYCODE_KP_MEMRECALL = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_MEMRECALL),
    KEY_KEYCODE_KP_MEMCLEAR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_MEMCLEAR),
    KEY_KEYCODE_KP_MEMADD = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_MEMADD),
    KEY_KEYCODE_KP_MEMSUBTRACT = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_MEMSUBTRACT),
    KEY_KEYCODE_KP_MEMMULTIPLY = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_MEMMULTIPLY),
    KEY_KEYCODE_KP_MEMDIVIDE = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_MEMDIVIDE),
    KEY_KEYCODE_KP_PLUSMINUS = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_PLUSMINUS),
    KEY_KEYCODE_KP_CLEAR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_CLEAR),
    KEY_KEYCODE_KP_CLEARENTRY = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_CLEARENTRY),
    KEY_KEYCODE_KP_BINARY = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_BINARY),
    KEY_KEYCODE_KP_OCTAL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_OCTAL),
    KEY_KEYCODE_KP_DECIMAL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KP_DECIMAL),
    KEY_KEYCODE_KP_HEXADECIMAL = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KP_HEXADECIMAL),

    KEY_KEYCODE_LCTRL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_LCTRL),
    KEY_KEYCODE_LSHIFT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_LSHIFT),
    KEY_KEYCODE_LALT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_LALT),
    KEY_KEYCODE_LGUI = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_LGUI),
    KEY_KEYCODE_RCTRL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RCTRL),
    KEY_KEYCODE_RSHIFT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RSHIFT),
    KEY_KEYCODE_RALT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RALT),
    KEY_KEYCODE_RGUI = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_RGUI),

    KEY_KEYCODE_MODE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_MODE),

    KEY_KEYCODE_AUDIONEXT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIONEXT),
    KEY_KEYCODE_AUDIOPREV = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIOPREV),
    KEY_KEYCODE_AUDIOSTOP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIOSTOP),
    KEY_KEYCODE_AUDIOPLAY = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIOPLAY),
    KEY_KEYCODE_AUDIOMUTE = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIOMUTE),
    KEY_KEYCODE_MEDIASELECT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_MEDIASELECT),
    KEY_KEYCODE_WWW = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_WWW),
    KEY_KEYCODE_MAIL = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_MAIL),
    KEY_KEYCODE_CALCULATOR = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_CALCULATOR),
    KEY_KEYCODE_COMPUTER = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_COMPUTER),
    KEY_KEYCODE_AC_SEARCH = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_SEARCH),
    KEY_KEYCODE_AC_HOME = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_HOME),
    KEY_KEYCODE_AC_BACK = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_BACK),
    KEY_KEYCODE_AC_FORWARD = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_FORWARD),
    KEY_KEYCODE_AC_STOP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_STOP),
    KEY_KEYCODE_AC_REFRESH = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AC_REFRESH),
    KEY_KEYCODE_AC_BOOKMARKS = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_AC_BOOKMARKS),

    KEY_KEYCODE_BRIGHTNESSDOWN = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_BRIGHTNESSDOWN),
    KEY_KEYCODE_BRIGHTNESSUP = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_BRIGHTNESSUP),
    KEY_KEYCODE_DISPLAYSWITCH = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_DISPLAYSWITCH),
    KEY_KEYCODE_KBDILLUMTOGGLE = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KBDILLUMTOGGLE),
    KEY_KEYCODE_KBDILLUMDOWN = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_KBDILLUMDOWN),
    KEY_KEYCODE_KBDILLUMUP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_KBDILLUMUP),
    KEY_KEYCODE_EJECT = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_EJECT),
    KEY_KEYCODE_SLEEP = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_SLEEP),
    KEY_KEYCODE_APP1 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_APP1),
    KEY_KEYCODE_APP2 = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_APP2),

    KEY_KEYCODE_AUDIOREWIND = KEY_SCANCODE_TO_KEYCODE(KEY_SCANCODE_AUDIOREWIND),
    KEY_KEYCODE_AUDIOFASTFORWARD = KEY_SCANCODE_TO_KEYCODE(
        KEY_SCANCODE_AUDIOFASTFORWARD)
} key_keycode;

#endif