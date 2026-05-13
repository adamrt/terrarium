// Copyright (c) 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "gfx/gfx.h"
#include "ui/ui.h"
#include "ws/ws.h"

// Forward declared
static gfx_rect_t scrollbar_thumb_rect(struct ui_scrollbar* scrollbar);

void func_event(struct ui_scrollbar* scrollbar, const ws_event_t* event)
{
    gfx_rect_t thumb_rect = scrollbar_thumb_rect(scrollbar);

    i32 max_scroll_height = i32_max(0, scrollbar->content_height - scrollbar->viewport_height);

    switch (event->type) {

    case WS_EVENT_MOUSEBUTTON_DOWN: {
        i32 mx = event->u.mousebutton.pos_x;
        i32 my = event->u.mousebutton.pos_y;

        if (gfx_rect_contains(thumb_rect, mx, my)) {
            scrollbar->thumb_grabbed = true;
            scrollbar->thumb_grab_offset_y = my - thumb_rect.y;
        }
    } break;

    case WS_EVENT_MOUSEBUTTON_UP:
        scrollbar->thumb_grabbed = false;
        scrollbar->thumb_grab_offset_y = 0;

        break;

    case WS_EVENT_MOUSEMOVE:
        if (scrollbar->thumb_grabbed) {
            i32 travel_distance = scrollbar->track_rect.height - thumb_rect.height;

            if (travel_distance > 0) {
                i32 thumb_y = event->u.mousemove.pos_y - scrollbar->thumb_grab_offset_y;
                thumb_y = i32_clamp(thumb_y, 0, travel_distance);

                f32 scroll_percent = (f32)thumb_y / (f32)travel_distance;
                i32 scroll_y = (i32)(scroll_percent * (f32)max_scroll_height);
                scrollbar->scroll_y = i32_clamp(scroll_y, 0, max_scroll_height);
            }
        }

    default:
        break;
    }
}

void func_update(struct ui_scrollbar* scrollbar, const ui_scrollbar_update_desc* update)
{
    ASSERT(scrollbar);
    ASSERT(update);

    if (!gfx_rect_is_empty(update->track_rect)) {
        scrollbar->track_rect = update->track_rect;
    }
    if (update->content_height > 0) {
        scrollbar->content_height = update->content_height;
    }
    if (update->viewport_height > 0) {
        scrollbar->viewport_height = update->viewport_height;
    }

    // FIXME: 0 is a valid case here, but if its empty its a problem
    if (update->scroll_y >= 0) {
        scrollbar->scroll_y = update->scroll_y;
    }
}

void func_draw(struct ui_scrollbar* scrollbar, gfx_surface_t* surface)
{

    gfx_surface_fill_rect(surface, scrollbar->track_rect, gfx_color_rgb(100, 100, 100));

    gfx_rect_t thumb_rect = scrollbar_thumb_rect(scrollbar);
    gfx_surface_fill_rect(surface, thumb_rect, gfx_white);
}

ui_scrollbar_t* ui_scrollbar_create(mem_allocator_t* alloc, gfx_rect_t track_rect, i32 content_height, i32 viewport_height)
{
    ASSERT(alloc);

    ui_scrollbar_t* scrollbar = mem_alloc(alloc, sizeof(*scrollbar));
    ASSERT(scrollbar);

    scrollbar->track_rect = track_rect;
    scrollbar->content_height = content_height;
    scrollbar->viewport_height = viewport_height;
    scrollbar->scroll_y = 0;
    scrollbar->thumb_grabbed = false;
    scrollbar->thumb_grab_offset_y = 0;

    scrollbar->func_event = func_event;
    scrollbar->func_update = func_update;
    scrollbar->func_draw = func_draw;

    return scrollbar;
}

void ui_scrollbar_destroy(mem_allocator_t* alloc, ui_scrollbar_t* scrollbar)
{
    ASSERT(alloc);
    ASSERT(scrollbar);

    mem_free(alloc, scrollbar);
}

static gfx_rect_t scrollbar_thumb_rect(struct ui_scrollbar* scrollbar)
{
    i32 track_height = scrollbar->track_rect.height;

    // Full size thumb
    i32 thumb_y = 0;
    i32 thumb_height = track_height;

    i32 max_scroll_height = i32_max(0, scrollbar->content_height - scrollbar->viewport_height);
    f32 visible_ratio = (f32)scrollbar->viewport_height / (f32)scrollbar->content_height;

    // Partial size thumb
    if (max_scroll_height > 0) {
        i32 thumb_min_height = 20;

        thumb_height = (i32)((f32)track_height * visible_ratio);
        thumb_height = i32_max(thumb_height, thumb_min_height);

        i32 current_scroll = i32_min(scrollbar->scroll_y, max_scroll_height);
        f32 scroll_percent = (f32)current_scroll / (f32)max_scroll_height;

        i32 travel_distance = track_height - thumb_height;
        thumb_y = (i32)(scroll_percent * (f32)travel_distance);
    }

    return (gfx_rect_t) {
        .x = scrollbar->track_rect.x,
        .y = thumb_y,
        .width = scrollbar->track_rect.width,
        .height = thumb_height,
    };
}
