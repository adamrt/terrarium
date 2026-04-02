#include "gfx/draw.h"
#include "ak/types.h"
#include "gfx/surface.h"

void gfx_draw_pixel(gfx_surface_t* surface, u32 x, u32 y, u32 color) {
    surface->data[y * surface->width + x] = color;
}
