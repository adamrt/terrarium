#pragma once

#include "ak/types.h"

typedef enum {
    OS_MOUSE_BUTTON_UNKNOWN = 0,
    OS_MOUSE_BUTTON_LEFT,
    OS_MOUSE_BUTTON_RIGHT,
    OS_MOUSE_BUTTON_MIDDLE,
} os_mouse_button_e;

typedef enum {
    OS_EVENT_UNKNOWN = 0,
    OS_EVENT_QUIT,

    OS_EVENT_MOUSE_MOVE,
    OS_EVENT_MOUSE_BUTTON,
    OS_EVENT_MOUSE_WHEEL,
} os_event_type_e;

// clang-format off
typedef struct {
    os_event_type_e type;

    union {
        struct { i32 pos_x, pos_y;                                            } mouse_move;
        struct { i32 pos_x, pos_y; bool is_pressed; os_mouse_button_e button; } mouse_button;
        struct { i32 pos_x, pos_y; f32 scroll_x, scroll_y;                    } mouse_wheel;
    } u;

} os_event_t;
// clang-format on

bool os_event_poll(os_event_t* out);
