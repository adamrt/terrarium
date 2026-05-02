// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "exp/logviewer/app.h"

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

#include <stdio.h>
#include <string.h>

static const char* LOG_FILE = "src/main.c";

enum {
    LINE_HEIGHT = GFX_FONT_HEIGHT + 2,
};

typedef struct {
    mem_allocator_t* alloc;
    gfx_color_t background_color;

    str_t contents;
    strview_t* lines;
    size_t line_count;

    i32 scroll_y;
} state_t;

static void read_logfile(state_t* state)
{
    // Contents needs to out live lines
    state->contents = io_file_readall(state->alloc, sv(LOG_FILE));
    state->lines = str_split_lines(state->alloc, state->contents, &state->line_count);
}

static void func_draw(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    gfx_surface_fill(window->content, state->background_color);
    for (i32 line = 0; line < (i32)state->line_count; line++) {
        if (line * LINE_HEIGHT > window->content->height) {
            break;
        }
        i32 index = line + state->scroll_y;
        index = i32_min(index, (i32)state->line_count - 1);
        gfx_surface_draw_text(window->content, 0, LINE_HEIGHT * line, state->lines[index], gfx_white);
    }
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

static void func_event(struct ws_window* window, const ws_event_t* event)
{
    ASSERT(window);
    ASSERT(event);

    state_t* state = window->ctx;

    state->scroll_y -= event->u.mousewheel.scroll_y;
    state->scroll_y = i32_max(0, state->scroll_y);
    state->scroll_y = i32_min(state->scroll_y, (i32)state->line_count - (window->content->height / LINE_HEIGHT) - 1);
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

    return window;
}
