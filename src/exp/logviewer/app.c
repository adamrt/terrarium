// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "exp/logviewer/app.h"

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

static const char* LOG_FILE = "src/main.c";

enum {
    LINE_HEIGHT = GFX_FONT_HEIGHT + 2,
    SCROLLBAR_TRACK_WIDTH = 14,
};

typedef struct {
    mem_allocator_t* alloc;
    gfx_color_t background_color;

    str_t contents;
    strview_t* lines;
    usize line_count;

    i32 scroll_y;
    bool thumb_grabbed;
    i32 thumb_grab_offset_y;
} state_t;

// Forward declarations
static void read_logfile(state_t* state);
static gfx_rect_t scrollbar_thumb_rect(ws_window_t* window);
static gfx_rect_t scrollbar_track_rect(ws_window_t* window);
static i32 get_max_scroll_height(ws_window_t* window);
static f32 get_visible_ratio(ws_window_t* window);

static void func_event(struct ws_window* window, const ws_event_t* event)
{
    ASSERT(window);
    ASSERT(event);

    state_t* state = window->ctx;

    switch (event->type) {
    case WS_EVENT_MOUSEWHEEL: {
        i32 max_scroll_height = get_max_scroll_height(window);

        state->scroll_y -= event->u.mousewheel.scroll_y * LINE_HEIGHT;
        state->scroll_y = i32_clamp(state->scroll_y, 0, i32_max(0, max_scroll_height));
    } break;

    case WS_EVENT_MOUSEBUTTON_DOWN: {
        i32 mx = event->u.mousebutton.pos_x;
        i32 my = event->u.mousebutton.pos_y;
        gfx_rect_t thumb_rect = scrollbar_thumb_rect(window);

        if (gfx_rect_contains(thumb_rect, mx, my)) {
            state->thumb_grabbed = true;
            state->thumb_grab_offset_y = my - thumb_rect.y;
        }
    } break;

    case WS_EVENT_MOUSEBUTTON_UP:
        state->thumb_grabbed = false;
        state->thumb_grab_offset_y = 0;
        break;

    case WS_EVENT_MOUSEMOVE:
        if (state->thumb_grabbed) {
            gfx_rect_t thumb_rect = scrollbar_thumb_rect(window);
            gfx_rect_t track_rect = scrollbar_track_rect(window);
            i32 travel_distance = track_rect.height - thumb_rect.height;

            if (travel_distance > 0) {
                i32 thumb_y = event->u.mousemove.pos_y - state->thumb_grab_offset_y;
                thumb_y = i32_clamp(thumb_y, 0, travel_distance);

                i32 max_scroll_height = get_max_scroll_height(window);
                f32 scroll_percent = (f32)thumb_y / (f32)travel_distance;
                state->scroll_y = (i32)(scroll_percent * (f32)max_scroll_height);
                state->scroll_y = i32_clamp(state->scroll_y, 0, max_scroll_height);
            }
        }
        break;

    default:
        break;
    }
}

static void func_draw(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    gfx_surface_fill(window->content, state->background_color);

    for (i32 line = 0; line < (i32)state->line_count; line++) {
        if (line * LINE_HEIGHT >= window->content->height) {
            break;
        }
        i32 index = line + state->scroll_y / LINE_HEIGHT;
        index = i32_min(index, (i32)state->line_count - 1);
        gfx_surface_draw_text(window->content, 0, LINE_HEIGHT * line, state->lines[index], gfx_white);
    }

    gfx_rect_t track_rect = scrollbar_track_rect(window);
    gfx_surface_fill_rect(window->content, track_rect, gfx_color_rgb(100, 100, 100));

    gfx_rect_t thumb_rect = scrollbar_thumb_rect(window);
    gfx_surface_fill_rect(window->content, thumb_rect, gfx_white);
}

static void func_close(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;
    mem_allocator_t* alloc = state->alloc; // Needed for ws_window_destroy

    str_destroy(alloc, state->contents);
    mem_free(alloc, state->lines);
    mem_free(alloc, window->ctx);
}

ws_window_t* exp_logviewer_create(mem_allocator_t* alloc, i32 x, i32 y)
{
    ASSERT(alloc);

    ws_window_t* window = ws_window_create(alloc, sv("Log Viewer App"), x, y, 300, 300);
    ASSERT(window);

    state_t* state = mem_alloc(alloc, sizeof(*state));
    ASSERT(state);

    state->alloc = alloc;
    state->background_color = gfx_black;
    state->line_count = 0;
    state->scroll_y = 0;
    state->thumb_grabbed = false;
    state->thumb_grab_offset_y = 0;

    window->func_event = func_event;
    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    read_logfile(state);

    return window;
}

static void read_logfile(state_t* state)
{
    // Contents needs to out live lines
    state->contents = io_file_readall(state->alloc, sv(LOG_FILE));
    state->lines = str_split_lines(state->alloc, state->contents, &state->line_count);

    ASSERT(state->line_count <= I32_MAX);
}

static i32 scrollbar_track_height(ws_window_t* window)
{
    return window->content->height - WS_FRAME_HANDLE_SIZE + WS_FRAME_PADDING_SIZE + (1 * WS_FRAME_BORDER_SIZE);
}

static gfx_rect_t scrollbar_track_rect(ws_window_t* window)
{
    ASSERT(window);

    i32 track_height = scrollbar_track_height(window);

    return (gfx_rect_t) {
        .x = window->content->width - SCROLLBAR_TRACK_WIDTH,
        .y = 0,
        .width = SCROLLBAR_TRACK_WIDTH,
        .height = track_height,
    };
}

static gfx_rect_t scrollbar_thumb_rect(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    i32 track_height = scrollbar_track_height(window);

    // Full size thumb
    i32 thumb_y = 0;
    i32 thumb_height = track_height;

    i32 max_scroll_height = get_max_scroll_height(window);
    f32 visible_ratio = get_visible_ratio(window);

    // Partial size thumb
    if (max_scroll_height > 0) {
        i32 thumb_min_height = 20;

        thumb_height = (i32)((f32)track_height * visible_ratio);
        thumb_height = i32_max(thumb_height, thumb_min_height);

        i32 current_scroll = i32_min(state->scroll_y, max_scroll_height);
        f32 scroll_percent = (f32)current_scroll / (f32)max_scroll_height;

        i32 travel_distance = track_height - thumb_height;
        thumb_y = (i32)(scroll_percent * (f32)travel_distance);
    }

    return (gfx_rect_t) {
        .x = window->content->width - SCROLLBAR_TRACK_WIDTH,
        .y = thumb_y,
        .width = SCROLLBAR_TRACK_WIDTH,
        .height = thumb_height,
    };
}

static i32 get_max_scroll_height(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    i32 content_height = (i32)state->line_count * LINE_HEIGHT;
    i32 viewport_height = window->content->height;
    i32 max_scroll = content_height - viewport_height;
    max_scroll = i32_max(0, max_scroll);

    return max_scroll;
}

static f32 get_visible_ratio(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    ASSERT(state->line_count > 0); // Avoid division by 0 below

    i32 content_height = (i32)state->line_count * LINE_HEIGHT;
    i32 viewport_height = window->content->height;
    return (f32)viewport_height / (f32)content_height;
}
