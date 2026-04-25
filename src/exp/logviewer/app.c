// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "exp/logviewer/app.h"

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

#include <stdio.h>
#include <string.h>

static const char* LOG_FILE = "./compile_commands.json";

enum {
    LINE_HEIGHT = GFX_FONT_HEIGHT + 2,
    MAX_LINES = 64,
    LINE_MAX_BYTES = 1024,
    TAIL_MAX_BYTES = LINE_MAX_BYTES * MAX_LINES,
};

typedef struct {
    mem_allocator_t* alloc;
    gfx_color_t background_color;

    char* buffer;
    strview_t lines[MAX_LINES];
    i32 line_count;
} state_t;

static void read_logfile(state_t* state)
{
    FILE* f = fopen(LOG_FILE, "r");
    ASSERT(f);
    {
        size_t len = fread(state->buffer, sizeof(char), TAIL_MAX_BYTES - 1, f);
        ASSERT(len > 0);
        state->buffer[len] = '\0';
    }
    ASSERT(fclose(f) == 0);

    state->line_count = 0;
    char* buffer_ptr = state->buffer;

    while (*buffer_ptr && state->line_count < MAX_LINES) {
        char* start = buffer_ptr;
        while (*buffer_ptr && *buffer_ptr != '\n') {
            buffer_ptr++;
        }

        if (buffer_ptr > start) {
            state->lines[state->line_count++] = (strview_t) {
                .ptr = start,
                .len = (size_t)(buffer_ptr - start),
            };
        }

        if (*buffer_ptr == '\n') {
            buffer_ptr++;
        }
    }
}

static void func_draw(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;

    gfx_surface_fill(window->content, state->background_color);
    for (i32 line = 0; line < state->line_count; line++) {
        gfx_surface_draw_strview(window->content, 0, LINE_HEIGHT * line, state->lines[line], gfx_white);
    }
}

static void func_close(ws_window_t* window)
{
    ASSERT(window);

    state_t* state = window->ctx;
    mem_allocator_t* alloc = state->alloc; // Needed for ws_window_destroy

    mem_free(alloc, state->buffer);
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
    state->buffer = mem_alloc(alloc, TAIL_MAX_BYTES);
    state->line_count = 0;

    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    read_logfile(state);

    return window;
}
