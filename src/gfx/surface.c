#include "gfx/surface.h"
#include "ak/types.h"

#include <assert.h>
#include <stdlib.h>

gfx_surface_t* gfx_surface_create(i32 width, i32 height) {
    assert(width > 0);
    assert(height > 0);

    gfx_surface_t* surface = calloc(1, sizeof(*surface));
    assert(surface);

    surface->data = calloc(1, width * height * sizeof(u32));
    assert(surface->data);

    surface->width = width;
    surface->height = height;

    return surface;
}

void gfx_surface_destroy(gfx_surface_t* surface) {
    assert(surface);
    assert(surface->data);

    free(surface->data);
    free(surface);
}

void gfx_surface_clear(gfx_surface_t* surface, u32 color) {
    for (i32 y = 0; y < surface->height; ++y) {
        for (i32 x = 0; x < surface->width; ++x) {
            surface->data[y * surface->width + x] = color;
        }
    }
}
