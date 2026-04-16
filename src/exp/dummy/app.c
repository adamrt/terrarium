// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "app.h"
#include "ak/ak.h"
#include "gfx/gfx.h"
#include "ws/ws.h"

typedef struct {
    mem_allocator_t* alloc;
    f32 timer;
    gfx_color_t color_a;
    gfx_color_t color_b;
} state_t;

void func_draw(ws_window_t* window)
{
    state_t* state = window->ctx;

    f32 t = (f32_sin(state->timer) + 1.0f) * 0.5f;

    gfx_color_t color = gfx_color_lerp(state->color_a, state->color_b, t);
    gfx_surface_fill(window->content, color);

    state->timer += 0.015f;
}

void func_close(ws_window_t* window)
{
    state_t* state = window->ctx;
    mem_allocator_t* alloc = state->alloc; // Needed for ws_window_destroy

    mem_free(alloc, window->ctx);
}

ws_window_t* exp_dummy_create(mem_allocator_t* alloc, i32 x, i32 y)
{
    ws_window_t* window = ws_window_create(alloc, x, y, 300, 300);
    ASSERT(window);

    state_t* state = mem_alloc(alloc, sizeof(*state));
    ASSERT(state);

    state->alloc = alloc;
    state->timer = 0.0f;
    state->color_a = gfx_color_random();
    state->color_b = gfx_color_random();

    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    return window;
}
