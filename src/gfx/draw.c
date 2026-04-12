// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"

// FIXME: This should take gfx_pixel_t instead of color. It will be called
// repeatedly and we don't want to pack the color each time.
void gfx_surface_draw_pixel(gfx_surface_t* surface, i32 x, i32 y, gfx_color_t color)
{
    ASSERT(surface);

    gfx_pixel_t packed = gfx_color_pack(color);
    surface->data[y * surface->width + x] = packed;
}

void gfx_surface_fill_rect(gfx_surface_t* surface, gfx_rect_t rect, gfx_color_t color)
{
    ASSERT(surface);

    gfx_pixel_t packed = gfx_color_pack(color);

    for (i32 y = rect.y; y < rect.y + rect.height; ++y) {
        for (i32 x = rect.x; x < rect.x + rect.width; ++x) {
            surface->data[y * surface->width + x] = packed;
        }
    }
}

// FIXME: This is temporary
void gfx_surface_draw_wallpaper(gfx_surface_t* surface, i32 width, i32 height)
{
    i32 gap = height / 6;
    i32 y1 = gap * 1;
    i32 y2 = gap * 2;
    i32 y3 = gap * 3;

    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y1, gfx_red);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y2, gfx_green);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_surface_draw_pixel(surface, x, y3, gfx_blue);
    }
}
