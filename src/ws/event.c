#include "ws/ws.h"

const char* ws_event_str(const ws_event_t* event)
{
    ASSERT(event);
    switch (event->type) {
    case WS_EVENT_UNKNOWN:
        return "WS_EVENT_UNKNOWN";
    case WS_EVENT_QUIT:
        return "WS_EVENT_QUIT";
    case WS_EVENT_MOUSEMOVE:
        return "WS_EVENT_MOUSEMOVE";
    case WS_EVENT_MOUSEBUTTON_DOWN:
        return "WS_EVENT_MOUSEBUTTON_DOWN";
    case WS_EVENT_MOUSEBUTTON_UP:
        return "WS_EVENT_MOUSEBUTTON_UP";
    case WS_EVENT_MOUSEWHEEL:
        return "WS_EVENT_MOUSEWHEEL";
    case WS_EVENT_KEY_DOWN:
        return "WS_EVENT_KEY_DOWN";
    case WS_EVENT_KEY_UP:
        return "WS_EVENT_KEY_UP";
    case WS_EVENT_WINDOW_ENTER:
        return "WS_EVENT_WINDOW_ENTER";
    case WS_EVENT_WINDOW_LEAVE:
        return "WS_EVENT_WINDOW_LEAVE";
    }

    __builtin_unreachable();
}
