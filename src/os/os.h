#pragma once

#include "ak/ak.h"
#include "gfx/gfx.h"

//
// Lifecycle
//

void os_init(void);
void os_shutdown(void);

//
// Display
//

typedef struct os_display os_display_t;

os_display_t* os_display_create(mem_allocator_t* alloc, i32 width, i32 height);
void os_display_destroy(mem_allocator_t* alloc, os_display_t* display);
void os_display_present(os_display_t* display, gfx_surface_t* surface);

//
// Event/Input Handling
//

typedef enum {
    OS_MOUSE_BUTTON_UNKNOWN = 0,
    OS_MOUSE_BUTTON_LEFT,
    OS_MOUSE_BUTTON_RIGHT,
    OS_MOUSE_BUTTON_MIDDLE,
} os_mouse_button_e;

// os_key_code_e represents the physical key -- not the logical key. SDL and
// every OS/input-system have to deal with physical and logical keys. The naming
// isn't 100% consistent between implementations but are somewhat similar. SDL
// calls them scancodes (physical) and keycodes (logical). We will eventually
// need both when we have our own OS, or sooner. But, for now we are only
// handling physical and prefer the naming "key code" over the name scancode.
// When we require both, this will most likely be renamed os_scancode_e though.
//
// clang-format off
typedef enum {
    OS_KEY_UNKNOWN = 0,

    OS_KEY_1, OS_KEY_2, OS_KEY_3, OS_KEY_4, OS_KEY_5,
    OS_KEY_6, OS_KEY_7, OS_KEY_8, OS_KEY_9, OS_KEY_0,

    OS_KEY_A, OS_KEY_B, OS_KEY_C, OS_KEY_D, OS_KEY_E, OS_KEY_F, OS_KEY_G,
    OS_KEY_H, OS_KEY_I, OS_KEY_J, OS_KEY_K, OS_KEY_L, OS_KEY_M, OS_KEY_N,
    OS_KEY_O, OS_KEY_P, OS_KEY_Q, OS_KEY_R, OS_KEY_S, OS_KEY_T, OS_KEY_U,
    OS_KEY_V, OS_KEY_W, OS_KEY_X, OS_KEY_Y, OS_KEY_Z,

    // Special
    OS_KEY_SPACE, OS_KEY_ESCAPE, OS_KEY_BACKSPACE, OS_KEY_TAB, OS_KEY_CAPSLOCK, OS_KEY_ENTER,

    // Symbols
    OS_KEY_GRAVE, OS_KEY_MINUS, OS_KEY_EQUALS, OS_KEY_LEFTBRACKET, OS_KEY_RIGHTBRACKET,
    OS_KEY_SEMICOLON, OS_KEY_APOSTROPHE, OS_KEY_COMMA, OS_KEY_PERIOD, OS_KEY_FORWARDSLASH, OS_KEY_BACKSLASH,

    // Modifiers
    OS_KEY_LEFTALT, OS_KEY_RIGHTALT, OS_KEY_LEFTCONTROL, OS_KEY_RIGHTCONTROL,
    OS_KEY_LEFTSHIFT, OS_KEY_RIGHTSHIFT, OS_KEY_LEFTSUPER, OS_KEY_RIGHTSUPER,

    // Arrows
    OS_KEY_UP, OS_KEY_DOWN, OS_KEY_LEFT, OS_KEY_RIGHT,

    OS_KEY__COUNT,
} os_key_code_e;
// clang-format on

typedef enum {
    OS_MOD_UNKNOWN = 0,

    OS_MOD_ALT,
    OS_MOD_CONTROL,
    OS_MOD_SHIFT,
    OS_MOD_SUPER,

    OS_MOD__COUNT,
} os_mod_e;

static char static_assert_key_code[(OS_KEY__COUNT == 66) ? 1 : -1];

typedef enum {
    OS_EVENT_UNKNOWN = 0,
    OS_EVENT_QUIT,

    OS_EVENT_MOUSE_MOVE,
    OS_EVENT_MOUSE_BUTTON,
    OS_EVENT_MOUSE_WHEEL,

    OS_EVENT_KEY,
} os_event_type_e;

// clang-format off
typedef struct {
    os_event_type_e type;

    union {
        struct { i32 pos_x, pos_y;                                             } mouse_move;
        struct { i32 pos_x, pos_y; os_mouse_button_e button; bool is_pressed;  } mouse_button;
        struct { i32 pos_x, pos_y; f32 scroll_x, scroll_y;                     } mouse_wheel;
        struct { os_key_code_e code; bool is_pressed; bool is_repeat; u8 mods; } key;
    } u;

} os_event_t;
// clang-format on

bool os_event_poll(os_event_t* out);

u8 os_mod_bitset(void);
void os_mod_bitset_set(os_mod_e code, bool is_down);
os_mod_e os_key_to_mod(os_key_code_e);
