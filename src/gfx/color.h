#pragma once

#include "ak/types.h"

typedef u32 gfx_pixel_t;

typedef struct {
    u8 r, g, b, a;
} gfx_color_t;

gfx_color_t gfx_color_rgb(u8 r, u8 g, u8 b);
gfx_color_t gfx_color_rgba(u8 r, u8 g, u8 b, u8 a);
gfx_color_t gfx_color_unpack(gfx_pixel_t color);
gfx_pixel_t gfx_color_pack(gfx_color_t color);

extern const gfx_color_t gfx_black;
extern const gfx_color_t gfx_white;
extern const gfx_color_t gfx_red;
extern const gfx_color_t gfx_green;
extern const gfx_color_t gfx_blue;
