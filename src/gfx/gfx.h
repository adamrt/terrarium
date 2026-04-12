// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#pragma once

#include "ak/ak.h"

//
// Color
//

// gfx_color_t is the user facing color type
typedef struct {
    u8 r, g, b, a;
} gfx_color_t;

// gfx_pixel_t is the packed backing store type
typedef u32 gfx_pixel_t;

gfx_color_t gfx_color_rgb(u8 r, u8 g, u8 b);
gfx_color_t gfx_color_rgba(u8 r, u8 g, u8 b, u8 a);
gfx_color_t gfx_color_scale(gfx_color_t color, f32 scale);
gfx_color_t gfx_color_lerp(gfx_color_t a, gfx_color_t b, f32 t);
gfx_color_t gfx_color_random(void);

// Convert gfx_color_t <-> gfx_pixel_t
gfx_color_t gfx_pixel_unpack(gfx_pixel_t color);
gfx_pixel_t gfx_color_pack(gfx_color_t color);

extern const gfx_color_t gfx_black;
extern const gfx_color_t gfx_white;
extern const gfx_color_t gfx_red;
extern const gfx_color_t gfx_green;
extern const gfx_color_t gfx_blue;

//
// Rectangle
//

typedef struct {
    i32 x, y;
    i32 width, height;
} gfx_rect_t;

bool gfx_rect_contains(gfx_rect_t rect, i32 x, i32 y);

//
// Surface
//

typedef struct {
    gfx_pixel_t* data;
    i32 width, height;
} gfx_surface_t;

gfx_surface_t* gfx_surface_create(mem_allocator_t* alloc, i32 width, i32 height);
void gfx_surface_destroy(mem_allocator_t* alloc, gfx_surface_t* surface);
void gfx_surface_blit(gfx_surface_t* target, const gfx_surface_t* source, i32 x, i32 y);
void gfx_surface_draw_wallpaper(gfx_surface_t* surface, i32 width, i32 height);

//
// Draw
//

void gfx_surface_fill(gfx_surface_t* surface, gfx_color_t color);
void gfx_surface_draw_pixel(gfx_surface_t* surface, i32 x, i32 y, gfx_pixel_t pixel);
void gfx_surface_fill_rect(gfx_surface_t* surface, gfx_rect_t rect, gfx_color_t color);
