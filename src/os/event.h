#pragma once

#include "ak/types.h"

typedef enum {
    OS_EVENT_UNKNOWN = 0,
    OS_EVENT_QUIT,
} os_event_type_e;

typedef struct {
    os_event_type_e type;
} os_event_t;

bool os_event_poll(os_event_t* out);
