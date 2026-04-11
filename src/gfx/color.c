#include "ak/ak.h"
#include "gfx/gfx.h"

const gfx_color_t gfx_black = { 0, 0, 0, 255 };
const gfx_color_t gfx_white = { 255, 255, 255, 255 };
const gfx_color_t gfx_red = { 255, 0, 0, 255 };
const gfx_color_t gfx_green = { 0, 255, 0, 255 };
const gfx_color_t gfx_blue = { 0, 0, 255, 255 };

gfx_color_t gfx_color_rgb(u8 r, u8 g, u8 b)
{
    return gfx_color_rgba(r, g, b, 255);
}

gfx_color_t gfx_color_rgba(u8 r, u8 g, u8 b, u8 a)
{
    return (gfx_color_t) { r, g, b, a };
}

gfx_color_t gfx_color_scale(gfx_color_t color, f32 scale)
{
    scale = f32_clamp(scale, 0.0f, 1.0f);

    return (gfx_color_t) {
        .r = (u8)((f32)color.r * scale),
        .g = (u8)((f32)color.g * scale),
        .b = (u8)((f32)color.b * scale),
        .a = (u8)((f32)color.a * scale),
    };
}

// (1.0f - t) * a + t * b
gfx_color_t gfx_color_lerp(gfx_color_t a, gfx_color_t b, f32 t)
{
    t = f32_clamp(t, 0.0f, 1.0f);
    f32 inv_t = 1.0f - t;

    return (gfx_color_t) {
        .r = (u8)(inv_t * (f32)a.r + t * (f32)b.r),
        .g = (u8)(inv_t * (f32)a.g + t * (f32)b.g),
        .b = (u8)(inv_t * (f32)a.b + t * (f32)b.b),
        .a = (u8)(inv_t * (f32)a.a + t * (f32)b.a),
    };
}

gfx_color_t gfx_color_random(void)
{
    u8 r = (u8)rnd_i32_range(0, 255);
    u8 g = (u8)rnd_i32_range(0, 255);
    u8 b = (u8)rnd_i32_range(0, 255);
    return gfx_color_rgb(r, g, b);
}

gfx_color_t gfx_color_unpack(gfx_pixel_t color)
{
    return (gfx_color_t) {
        .r = (color >> 24) & 0xFF,
        .g = (color >> 16) & 0xFF,
        .b = (color >> 8) & 0xFF,
        .a = (color >> 0) & 0xFF,
    };
}

gfx_pixel_t gfx_color_pack(gfx_color_t c)
{
    return (gfx_pixel_t)(((u32)c.r << 24) | ((u32)c.g << 16) | ((u32)c.b << 8) | ((u32)c.a << 0));
}
