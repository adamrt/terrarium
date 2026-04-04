#pragma once

#include "ak/types.h"

typedef enum {
    OS_EVENT_UNKNOWN = 0,
    OS_EVENT_QUIT,

    OS_EVENT_MOUSE_MOVE,
} os_event_type_e;

// clang-format off
typedef struct {
    os_event_type_e type;

    union {
        struct { i32 x, y; } mouse_move;
    } u;

} os_event_t;
// clang-format on

bool os_event_poll(os_event_t* out);
