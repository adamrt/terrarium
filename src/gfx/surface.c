#include <stdlib.h>

#include "ak/ak.h"
#include "gfx/gfx.h"

gfx_surface_t* gfx_surface_create(mem_allocator_t* alloc, i32 width, i32 height)
{
    ASSERT(width > 0);
    ASSERT(height > 0);

    gfx_surface_t* surface = mem_alloc(alloc, sizeof(*surface));
    surface->data = mem_alloc(alloc, (size_t)width * (size_t)height * sizeof(u32));

    surface->width = width;
    surface->height = height;

    return surface;
}

void gfx_surface_destroy(mem_allocator_t* alloc, gfx_surface_t* surface)
{
    ASSERT(surface);
    ASSERT(surface->data);

    mem_free(alloc, surface->data);
    mem_free(alloc, surface);
}

void gfx_surface_clear(gfx_surface_t* surface, gfx_color_t color)
{
    ASSERT(surface);

    gfx_pixel_t packed = gfx_color_pack(color);

    for (i32 y = 0; y < surface->height; ++y) {
        for (i32 x = 0; x < surface->width; ++x) {
            surface->data[y * surface->width + x] = packed;
        }
    }
}

// FIXME: This can be much faster by memcpy'ing each row.
void gfx_surface_blit(gfx_surface_t* target, const gfx_surface_t* source, i32 target_x, i32 target_y)
{
    ASSERT(target);
    ASSERT(source);

    target_x = i32_clamp(target_x, 0, target->width - 1);
    target_y = i32_clamp(target_y, 0, target->height - 1);

    i32 source_x_end = i32_clamp(source->width, target_x, source->width);
    i32 source_y_end = i32_clamp(source->height, target_y, source->height);

    for (i32 source_y = 0; source_y < source_y_end; ++source_y) {
        for (i32 source_x = 0; source_x < source_x_end; ++source_x) {
            i32 tx = target_x + source_x;
            i32 ty = target_y + source_y;
            target->data[ty * target->width + tx] = source->data[source_y * source->width + source_x];
        }
    }
}

void gfx_surface_draw_wallpaper(gfx_surface_t* surface, i32 width, i32 height)
{
    i32 gap = height / 6;
    i32 y1 = gap * 1;
    i32 y2 = gap * 2;
    i32 y3 = gap * 3;

    for (i32 x = 0; x < width; ++x) {
        gfx_draw_pixel(surface, x, y1, gfx_red);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_draw_pixel(surface, x, y2, gfx_green);
    }
    for (i32 x = 0; x < width; ++x) {
        gfx_draw_pixel(surface, x, y3, gfx_blue);
    }
}
