// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#pragma once

#include "ak/ak.h"
#include "gfx/gfx.h"
#include "os/os.h"

//
// Window
//

typedef struct ws_window {
    gfx_rect_t rect;
    gfx_surface_t* content;

    void (*func_draw)(struct ws_window* window);
    void (*func_close)(struct ws_window* window);

    void* ctx;
} ws_window_t;

ws_window_t* ws_window_create(mem_allocator_t* alloc, i32 x, i32 y, i32 width, i32 height);
void ws_window_destroy(mem_allocator_t* alloc, ws_window_t* window);
gfx_rect_t ws_window_rect_content(const ws_window_t* window);

//
// Event
//

typedef enum {
    // os mirror
    WS_EVENT_UNKNOWN = 0,
    WS_EVENT_QUIT,
    WS_EVENT_MOUSEMOVE,
    WS_EVENT_MOUSEBUTTON_DOWN,
    WS_EVENT_MOUSEBUTTON_UP,
    WS_EVENT_MOUSEWHEEL,
    WS_EVENT_KEY_DOWN,
    WS_EVENT_KEY_UP,

    // ws specific
    WS_EVENT_WINDOW_ENTER,
    WS_EVENT_WINDOW_LEAVE,
} ws_event_type_e;

// clang-format off
typedef struct {
    ws_event_type_e type;

    union {
        os_event_mousemove_t mousemove;
        os_event_mousebutton_t mousebutton;
        os_event_mousewheel_t mousewheel;
        os_event_key_t key;
    } u;

} ws_event_t;

typedef enum {
    WS_HIT_NONE,
    WS_HIT_FRAME,
    WS_HIT_CONTENT,
} ws_hit_type_e;

typedef struct {
    ws_window_t* window;
    ws_hit_type_e type;
} ws_hit_t;

//
// Window Server
//

typedef struct ws_server ws_server_t;

ws_server_t* ws_server_create(mem_allocator_t* alloc, i32 width, i32 height);
void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server);
void ws_server_render(ws_server_t* server);
void ws_server_window_take(ws_server_t* server, ws_window_t** window);
void ws_server_event_handle(ws_server_t* server, const os_event_t* os_event);
