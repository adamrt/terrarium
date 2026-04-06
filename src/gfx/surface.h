#pragma once

#include "ak/mem.h"
#include "ak/types.h"
#include "gfx/color.h"

typedef struct {
    gfx_pixel_t* data;
    i32 width, height;
} gfx_surface_t;

gfx_surface_t* gfx_surface_create(mem_allocator_t* alloc, i32 width, i32 height);
void gfx_surface_destroy(mem_allocator_t* alloc, gfx_surface_t* surface);
void gfx_surface_clear(gfx_surface_t* surface, gfx_color_t color);

void gfx_surface_blit(gfx_surface_t* target, const gfx_surface_t* source, i32 x, i32 y);
