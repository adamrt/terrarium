// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

enum {
    WS_FRAME_BORDER_SIZE = 1,
    WS_FRAME_PADDING_SIZE = 10,
};

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

void ws_window_move(ws_window_t* window, i32 x, i32 y)
{
    ASSERT(window);

    window->rect.x = x;
    window->rect.y = y;
}

void ws_window_resize(mem_allocator_t* alloc, ws_window_t* window, i32 width, i32 height)
{
    ASSERT(alloc);
    ASSERT(window);

    width = i32_max(width, 50);
    height = i32_max(height, 50);

    if (window->rect.width == width && window->rect.height == height) {
        return;
    }

    window->rect.width = width;
    window->rect.height = height;

    gfx_rect_t content_rect = ws_window_rect_content(window);
    gfx_surface_resize(alloc, window->content, content_rect.width, content_rect.height);
}

gfx_rect_t ws_window_rect_total(const ws_window_t* window)
{
    return window->rect;
}

gfx_rect_t ws_window_rect_frame(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_total(window);
    rect.x += WS_FRAME_BORDER_SIZE;
    rect.y += WS_FRAME_BORDER_SIZE;
    rect.width -= (WS_FRAME_BORDER_SIZE * 2);
    rect.height -= (WS_FRAME_BORDER_SIZE * 2);
    return rect;
}

gfx_rect_t ws_window_rect_content_border(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_frame(window);
    rect.x += WS_FRAME_PADDING_SIZE;
    rect.y += WS_FRAME_PADDING_SIZE;
    rect.width -= (WS_FRAME_PADDING_SIZE * 2);
    rect.height -= (WS_FRAME_PADDING_SIZE * 2);
    return rect;
}

gfx_rect_t ws_window_rect_content(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_content_border(window);
    rect.x += WS_FRAME_BORDER_SIZE;
    rect.y += WS_FRAME_BORDER_SIZE;
    rect.width -= (WS_FRAME_BORDER_SIZE * 2);
    rect.height -= (WS_FRAME_BORDER_SIZE * 2);
    return rect;
}

gfx_rect_t ws_window_rect_handle_resize(const ws_window_t* window)
{
    i32 handle_dim = 20;

    gfx_rect_t rect = ws_window_rect_frame(window);
    rect.x = rect.x + rect.width - handle_dim;
    rect.y = rect.y + rect.height - handle_dim;
    rect.width = handle_dim;
    rect.height = handle_dim;
    return rect;
}
