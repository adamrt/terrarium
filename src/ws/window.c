// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

enum {
    WS_FRAME_BORDER_SIZE = 1,
    WS_FRAME_PADDING_SIZE = 10,
};

gfx_rect_t ws_window_rect_content(const ws_window_t* window)
{
    ASSERT(window);

    i32 pos_add = (2 * WS_FRAME_BORDER_SIZE + WS_FRAME_PADDING_SIZE);
    i32 dim_sub = (4 * WS_FRAME_BORDER_SIZE + 2 * WS_FRAME_PADDING_SIZE);

    return (gfx_rect_t) {
        .x = window->rect.x + pos_add,
        .y = window->rect.y + pos_add,
        .width = window->rect.width - dim_sub,
        .height = window->rect.height - dim_sub,
    };
}

ws_window_t* ws_window_create(mem_allocator_t* alloc, i32 x, i32 y, i32 width, i32 height)
{
    ASSERT(alloc);
    ASSERT(x >= 0)
    ASSERT(y >= 0)

    ws_window_t* window = mem_alloc(alloc, sizeof(*window));
    ASSERT(window);

    window->rect.x = x;
    window->rect.y = y;
    window->rect.width = width;
    window->rect.height = height;

    gfx_rect_t content_rect = ws_window_rect_content(window);
    gfx_surface_t* content = gfx_surface_create(alloc, content_rect.width, content_rect.height);
    ASSERT(content);

    window->content = content;

    return window;
}

void ws_window_destroy(mem_allocator_t* alloc, ws_window_t* window)
{
    ASSERT(alloc);
    ASSERT(window);

    gfx_surface_destroy(alloc, window->content);
    mem_free(alloc, window);
}
