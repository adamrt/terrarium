#include "gfx/surface.h"
#include "ak/assert.h"
#include "ak/mem.h"
#include "ak/types.h"
#include "gfx/color.h"

#include <stdlib.h>

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

void gfx_surface_blit(gfx_surface_t* target, const gfx_surface_t* source, i32 x, i32 y)
{
    i32 dx_start = x;
    i32 dy_start = y;
    i32 dx_end = dx_start + source->width;
    i32 dy_end = dy_start + source->height;

    i32 sx = 0;
    i32 sy = 0;

    for (i32 dy = dy_start; dy < dy_end; ++dy) {
        for (i32 dx = dx_start; dx < dx_end; ++dx) {
            i32 d_index = dy * target->width + dx;
            i32 s_index = sy * source->width + sx;
            target->data[d_index] = source->data[s_index];
            sx++;
        }
        sy++;
        sx = 0;
    }
}
