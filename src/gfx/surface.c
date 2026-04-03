#include "gfx/surface.h"
#include "ak/assert.h"
#include "ak/types.h"
#include "gfx/color.h"

#include <stdlib.h>

gfx_surface_t* gfx_surface_create(i32 width, i32 height)
{
    ASSERT(width > 0);
    ASSERT(height > 0);

    gfx_surface_t* surface = calloc(1, sizeof(*surface));
    ASSERT(surface);

    surface->data = calloc(1, (size_t)width * (size_t)height * sizeof(u32));
    ASSERT(surface->data);

    surface->width = width;
    surface->height = height;

    return surface;
}

void gfx_surface_destroy(gfx_surface_t* surface)
{
    ASSERT(surface);
    ASSERT(surface->data);

    free(surface->data);
    free(surface);
}

void gfx_surface_clear(gfx_surface_t* surface, gfx_color_t color)
{
    ASSERT(surface);

    for (i32 y = 0; y < surface->height; ++y) {
        for (i32 x = 0; x < surface->width; ++x) {
            surface->data[y * surface->width + x] = color;
        }
    }
}
