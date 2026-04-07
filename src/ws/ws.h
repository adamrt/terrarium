#pragma once

#include "ak/ak.h"
#include "gfx/gfx.h"

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

//
// Window Server
//

typedef struct ws_server ws_server_t;

ws_server_t* ws_server_create(mem_allocator_t* alloc, i32 width, i32 height);
void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server);
void ws_server_render(ws_server_t* server);
void ws_server_window_take(ws_server_t* server, ws_window_t* window);
