// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "gfx/gfx.h"

#include <stdlib.h>

gfx_surface_t* gfx_surface_create(mem_allocator_t* alloc, i32 width, i32 height)
{
    ASSERT(width > 0);
    ASSERT(height > 0);

    gfx_surface_t* surface = mem_alloc(alloc, sizeof(*surface));
    surface->data = mem_alloc(alloc, (size_t)width * (size_t)height * sizeof(u32));

    surface->width = width;
    surface->height = height;
    surface->width_capacity = width;
    surface->height_capacity = height;

    return surface;
}

void gfx_surface_destroy(mem_allocator_t* alloc, gfx_surface_t* surface)
{
    ASSERT(surface);
    ASSERT(surface->data);

    mem_free(alloc, surface->data);
    mem_free(alloc, surface);
}

void gfx_surface_resize(mem_allocator_t* alloc, gfx_surface_t* surface, i32 width, i32 height)
{
    ASSERT(alloc);
    ASSERT(surface);

    if (width <= surface->width_capacity && height <= surface->height_capacity) {
        surface->width = width;
        surface->height = height;
        return;
    }

    surface->width = width;
    surface->height = height;
    surface->width_capacity = i32_max(width, surface->width_capacity * 2);
    surface->height_capacity = i32_max(height, surface->height_capacity * 2);

    size_t new_size = (size_t)surface->width_capacity * (size_t)surface->height_capacity * sizeof(gfx_pixel_t);
    surface->data = mem_realloc(alloc, surface->data, new_size);
    ASSERT(surface->data);
}

// FIXME: This can be much faster by memcpy'ing each row.
void gfx_surface_blit(gfx_surface_t* target, const gfx_surface_t* source, i32 target_x, i32 target_y)
{
    ASSERT(target);
    ASSERT(source);

    // target_x = -5
    // source_x_start = 5
    i32 source_x_start = i32_max(0, -target_x);
    i32 source_y_start = i32_max(0, -target_y);

    // target->width = 100
    // source->width = 20
    // target_x = 95
    // source_x_end = 5
    i32 source_x_end = i32_min(source->width, target->width - target_x);
    i32 source_y_end = i32_min(source->height, target->height - target_y);

    if (source_x_start > source_x_end || source_y_start > source_y_end) {
        return;
    }

    for (i32 source_y = source_y_start; source_y < source_y_end; ++source_y) {
        for (i32 source_x = source_x_start; source_x < source_x_end; ++source_x) {
            i32 tx = target_x + source_x;
            i32 ty = target_y + source_y;
            target->data[ty * target->width + tx] = source->data[source_y * source->width + source_x];
        }
    }
}
