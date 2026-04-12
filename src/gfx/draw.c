// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"

void gfx_surface_draw_pixel(gfx_surface_t* surface, i32 x, i32 y, gfx_pixel_t pixel)
{
    ASSERT(surface);

    // You can return early instead of asserting, but we want our caller
    // functions to handle their own overdraw.
    ASSERT(x >= 0);
    ASSERT(y >= 0);
    ASSERT(x < surface->width);
    ASSERT(y < surface->height);

    surface->data[y * surface->width + x] = pixel;
}

void gfx_surface_fill_rect(gfx_surface_t* surface, gfx_rect_t rect, gfx_color_t color)
{
    ASSERT(surface);

    gfx_pixel_t packed = gfx_color_pack(color);

    i32 x_start = i32_max(0, rect.x);
    i32 y_start = i32_max(0, rect.y);

    i32 x_end = i32_min(surface->width, rect.x + rect.width);
    i32 y_end = i32_min(surface->height, rect.y + rect.height);

    if (x_start >= x_end || y_start >= y_end) {
        return;
    }

    for (i32 y = y_start; y < y_end; ++y) {
        for (i32 x = x_start; x < x_end; ++x) {
            surface->data[y * surface->width + x] = packed;
        }
    }
}

void gfx_surface_draw_wallpaper(gfx_surface_t* surface, i32 width, i32 height)
{
    ASSERT(surface);

    i32 gap = height / 6;
    i32 y1 = gap * 1;
    i32 y2 = gap * 2;
    i32 y3 = gap * 3;

    u32 red = gfx_color_pack(gfx_red);
    u32 green = gfx_color_pack(gfx_green);
    u32 blue = gfx_color_pack(gfx_blue);

    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y1, red);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y2, green);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y3, blue);
    }
}
