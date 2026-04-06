#include "gfx/draw.h"
#include "ak/assert.h"
#include "ak/types.h"
#include "gfx/color.h"
#include "gfx/surface.h"

void gfx_draw_pixel(gfx_surface_t* surface, i32 x, i32 y, gfx_color_t color)
{
    ASSERT(surface);

    gfx_pixel_t packed = gfx_color_pack(color);
    surface->data[y * surface->width + x] = packed;
}
