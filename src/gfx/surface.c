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
