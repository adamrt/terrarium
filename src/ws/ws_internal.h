// Copyright (c) 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#pragma once

#include "ws/ws.h"

gfx_rect_t ws_window_rect_total(const ws_window_t* window);
gfx_rect_t ws_window_rect_frame(const ws_window_t* window);
gfx_rect_t ws_window_rect_titlebar(const ws_window_t* window);
gfx_rect_t ws_window_rect_button_close(const ws_window_t* window);
gfx_rect_t ws_window_rect_button_maximize(const ws_window_t* window);
gfx_rect_t ws_window_rect_content_border(const ws_window_t* window);
gfx_rect_t ws_window_rect_content(const ws_window_t* window);
gfx_rect_t ws_window_rect_handle_resize(const ws_window_t* window);
