// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "exp/dummy/app.h"

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "os/os.h"
#include "ws/ws.h"

typedef struct {
    mem_allocator_t* alloc;
    f32 timer;
    gfx_color_t color_a;
    gfx_color_t color_b;

    i32 frames;
    i32 hit_x, hit_y;
} state_t;

static void func_event(ws_window_t* window, const ws_event_t* event)
{
    ASSERT(window);
    ASSERT(event);

    state_t* state = window->ctx;

    if (event->type == WS_EVENT_MOUSEBUTTON_DOWN && event->u.mousebutton.button == OS_MOUSE_BUTTON_LEFT) {
        state->hit_x = event->u.mousebutton.pos_x;
        state->hit_y = event->u.mousebutton.pos_y;
        state->frames = 60;
    }
}

static void func_draw(ws_window_t* window)
{
    state_t* state = window->ctx;

    f32 t = (f32_sin(state->timer) + 1.0f) * 0.5f;

    gfx_color_t color = gfx_color_lerp(state->color_a, state->color_b, t);
    gfx_surface_fill(window->content, color);

    if (state->frames > 0) {
        char pos_str[10];
        snprintf(pos_str, 10, "%dx%d\n", state->hit_x, state->hit_y);
        gfx_surface_draw_text(window->content, state->hit_x, state->hit_y, sv(pos_str), gfx_white);
        state->frames--;
    }

    state->timer += 0.015f;
}

static void func_close(ws_window_t* window)
{
    state_t* state = window->ctx;
    mem_allocator_t* alloc = state->alloc; // Needed for ws_window_destroy

    mem_free(alloc, window->ctx);
}

ws_window_t* exp_dummy_create(mem_allocator_t* alloc, i32 x, i32 y)
{
    ws_window_t* window = ws_window_create(alloc, sv("Dummy App"), x, y, 300, 300);
    ASSERT(window);

    state_t* state = mem_alloc(alloc, sizeof(*state));
    ASSERT(state);

    state->alloc = alloc;
    state->timer = 0.0f;
    state->color_a = gfx_color_random();
    state->color_b = gfx_color_random();
    state->frames = 0;
    state->hit_x = -1;
    state->hit_y = -1;

    window->func_event = func_event;
    window->func_draw = func_draw;
    window->func_close = func_close;
    window->ctx = state;

    return window;
}
