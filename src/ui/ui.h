// Copyright (c) 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#pragma once

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

typedef struct {
    gfx_rect_t track_rect;
    i32 content_height;
    i32 viewport_height;
    i32 scroll_y;
} ui_scrollbar_update_desc;

typedef struct ui_scrollbar {
    // Passed in
    gfx_rect_t track_rect;
    i32 content_height;
    i32 viewport_height;
    i32 scroll_y;

    // Internal properties
    bool thumb_grabbed;
    i32 thumb_grab_offset_y;

    // Internal methods
    void (*func_event)(struct ui_scrollbar* scrollbar, const ws_event_t* event);
    void (*func_update)(struct ui_scrollbar* scrollbar, const ui_scrollbar_update_desc* update);
    void (*func_draw)(struct ui_scrollbar* scrollbar, gfx_surface_t* surface);
} ui_scrollbar_t;

ui_scrollbar_t* ui_scrollbar_create(mem_allocator_t* alloc, gfx_rect_t track_rect, i32 content_height, i32 viewport_height);
void ui_scrollbar_destroy(mem_allocator_t* alloc, ui_scrollbar_t* widget);
