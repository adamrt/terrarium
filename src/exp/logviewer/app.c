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
    for (size_t line = 0; line < state->line_count; line++) {
        gfx_surface_draw_strview(window->content, 0, LINE_HEIGHT * (i32)line, state->lines[line], gfx_white);
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

ws_window_t* exp_logviewer_create(mem_allocator_t* alloc, i32 x, i32 y)
{
    ASSERT(alloc);

    ws_window_t* window = ws_window_create(alloc, "Log Viewer App", x, y, 300, 300);
    ASSERT(window);

    state_t* state = mem_alloc(alloc, sizeof(*state));
    ASSERT(state);

    state->alloc = alloc;
    state->background_color = gfx_black;
    state->line_count = 0;

    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    read_logfile(state);

    return window;
}
