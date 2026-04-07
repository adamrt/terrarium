#pragma once

#include "ak/mem.h"
#include "ak/types.h"
#include "gfx/color.h"
#include "gfx/rect.h"
#include "gfx/surface.h"

typedef struct ws_window ws_window_t;

// FIXME: Move this into .c
struct ws_window {
    gfx_rect_t rect;
    gfx_color_t content_color;

    gfx_surface_t* content;
};

ws_window_t* ws_window_create(mem_allocator_t* alloc, i32 x, i32 y, i32 width, i32 height);
void ws_window_destroy(mem_allocator_t* alloc, ws_window_t* window);
