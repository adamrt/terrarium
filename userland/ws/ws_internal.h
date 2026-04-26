#pragma once

#include "ws/ws.h"

enum {
    WS_FRAME_BORDER_SIZE = 1,
    WS_FRAME_PADDING_SIZE = 8,
    WS_FRAME_TITLEBAR_HEIGHT = 24,
    WS_FRAME_BUTTON_SIZE = 16,
    WS_FRAME_HANDLE_SIZE = 24,
};

gfx_rect_t ws_window_rect_total(const ws_window_t* window);
gfx_rect_t ws_window_rect_frame(const ws_window_t* window);
gfx_rect_t ws_window_rect_titlebar(const ws_window_t* window);
gfx_rect_t ws_window_rect_button_close(const ws_window_t* window);
gfx_rect_t ws_window_rect_button_maximize(const ws_window_t* window);
gfx_rect_t ws_window_rect_content_border(const ws_window_t* window);
gfx_rect_t ws_window_rect_content(const ws_window_t* window);
gfx_rect_t ws_window_rect_handle_resize(const ws_window_t* window);
