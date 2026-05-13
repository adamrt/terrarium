// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "exp/logviewer/app.h"

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ui/ui.h"
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

    ui_scrollbar_t* scrollbar;

    i32 scroll_y;
} state_t;

// Forward declarations
static void read_logfile(state_t* state);
static gfx_rect_t scrollbar_track_rect(ws_window_t* window);

static void func_event(struct ws_window* window, const ws_event_t* event)
{
    ASSERT(window);
    ASSERT(event);

    state_t* state = window->ctx;

    switch (event->type) {
    case WS_EVENT_WINDOW_RESIZE: {
        i32 content_height = (i32)state->line_count * LINE_HEIGHT;
        i32 viewport_height = window->content->height;
        i32 max_scroll = i32_max(0, content_height - viewport_height);

        state->scroll_y = i32_clamp(state->scroll_y, 0, max_scroll);
        state->scrollbar->func_update(state->scrollbar, &(ui_scrollbar_update_desc) {
                                                            .track_rect = scrollbar_track_rect(window),
                                                            .content_height = content_height,
                                                            .viewport_height = viewport_height,
                                                            .scroll_y = state->scroll_y,
                                                        });
    } break;

    case WS_EVENT_MOUSEWHEEL: {
        i32 content_height = (i32)state->line_count * LINE_HEIGHT;
        i32 viewport_height = window->content->height;
        i32 max_scroll = i32_max(0, content_height - viewport_height);

        state->scroll_y -= event->u.mousewheel.scroll_y * LINE_HEIGHT;
        state->scroll_y = i32_clamp(state->scroll_y, 0, max_scroll);
        state->scrollbar->func_update(state->scrollbar, &(ui_scrollbar_update_desc) { .scroll_y = state->scroll_y });
    } break;

    case WS_EVENT_MOUSEBUTTON_DOWN: {
        i32 mx = event->u.mousebutton.pos_x;
        i32 my = event->u.mousebutton.pos_y;
        if (gfx_rect_contains(scrollbar_track_rect(window), mx, my)) {
            state->scrollbar->func_event(state->scrollbar, event);
        }
    } break;

    case WS_EVENT_MOUSEBUTTON_UP:
        state->scrollbar->func_event(state->scrollbar, event);
        break;

    case WS_EVENT_MOUSEMOVE:
        state->scrollbar->func_event(state->scrollbar, event);
        // FIXME: This should be set by a callback, not reaching in directly.
        state->scroll_y = state->scrollbar->scroll_y;
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

    state->scrollbar->func_draw(state->scrollbar, window->content);
}

static void func_close(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;
    mem_allocator_t* alloc = state->alloc; // Needed for ws_window_destroy

    ui_scrollbar_destroy(alloc, state->scrollbar);
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

    window->func_event = func_event;
    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    read_logfile(state);

    gfx_rect_t track_rect = scrollbar_track_rect(window);
    i32 content_height = (i32)state->line_count * LINE_HEIGHT;
    i32 viewport_height = window->content->height;

    ui_scrollbar_t* scrollbar = ui_scrollbar_create(alloc, track_rect, content_height, viewport_height);
    ASSERT(scrollbar);
    state->scrollbar = scrollbar;

    return window;
}

static void read_logfile(state_t* state)
{
    // Contents needs to out live lines
    state->contents = io_file_readall(state->alloc, sv(LOG_FILE));
    state->lines = str_split_lines(state->alloc, state->contents, &state->line_count);

    ASSERT(state->line_count <= I32_MAX);
}

static gfx_rect_t scrollbar_track_rect(ws_window_t* window)
{
    ASSERT(window);

    i32 track_height = window->content->height - WS_FRAME_HANDLE_SIZE + WS_FRAME_PADDING_SIZE + (1 * WS_FRAME_BORDER_SIZE);

    return (gfx_rect_t) {
        .x = window->content->width - SCROLLBAR_TRACK_WIDTH,
        .y = 0,
        .width = SCROLLBAR_TRACK_WIDTH,
        .height = track_height,
    };
}
