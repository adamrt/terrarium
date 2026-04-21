// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"
#include "ws/ws_internal.h"

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
    window->restore_rect = (gfx_rect_t) { 0 };
    window->is_maximized = false;

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

void ws_window_close(mem_allocator_t* alloc, ws_window_t* window)
{
    ASSERT(alloc);
    ASSERT(window);

    window->func_close(window);
    ws_window_destroy(alloc, window);
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

    width = i32_max(width, 100);
    height = i32_max(height, 100);

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

gfx_rect_t ws_window_rect_titlebar(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_frame(window);
    rect.x += WS_FRAME_PADDING_SIZE;
    rect.y += WS_FRAME_PADDING_SIZE;
    rect.width -= (WS_FRAME_PADDING_SIZE * 2);
    rect.height = WS_FRAME_TITLEBAR_HEIGHT;
    return rect;
}

gfx_rect_t ws_window_rect_button_close(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_titlebar(window);
    rect.x += 0;
    rect.y += 0;
    rect.width = WS_FRAME_BUTTON_SIZE;
    rect.height = WS_FRAME_BUTTON_SIZE;
    return rect;
}

gfx_rect_t ws_window_rect_button_maximize(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_titlebar(window);
    rect.x += rect.width - WS_FRAME_BUTTON_SIZE;
    rect.y += 0;
    rect.width = WS_FRAME_BUTTON_SIZE;
    rect.height = WS_FRAME_BUTTON_SIZE;
    return rect;
}

gfx_rect_t ws_window_rect_content_border(const ws_window_t* window)
{
    gfx_rect_t rect = ws_window_rect_frame(window);
    rect.x += WS_FRAME_PADDING_SIZE;
    rect.y += WS_FRAME_PADDING_SIZE + WS_FRAME_TITLEBAR_HEIGHT;
    rect.width -= (WS_FRAME_PADDING_SIZE * 2);
    rect.height -= (WS_FRAME_PADDING_SIZE * 2) + WS_FRAME_TITLEBAR_HEIGHT;
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
    gfx_rect_t rect = ws_window_rect_frame(window);
    rect.x = rect.x + rect.width - WS_FRAME_HANDLE_SIZE;
    rect.y = rect.y + rect.height - WS_FRAME_HANDLE_SIZE;
    rect.width = WS_FRAME_HANDLE_SIZE;
    rect.height = WS_FRAME_HANDLE_SIZE;
    return rect;
}
