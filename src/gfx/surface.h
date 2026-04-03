#pragma once

#include "ak/types.h"
#include "gfx/color.h"

typedef struct {
    gfx_color_t* data;
    i32 width, height;
} gfx_surface_t;

gfx_surface_t* gfx_surface_create(i32 width, i32 height);
void gfx_surface_destroy(gfx_surface_t* surface);
void gfx_surface_clear(gfx_surface_t* surface, gfx_color_t color);
