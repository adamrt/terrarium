#include "gfx/rect.h"

bool gfx_rect_contains(gfx_rect_t rect, i32 x, i32 y)
{
    return (x >= rect.x && x < rect.x + rect.width) && (y >= rect.y && y < rect.y + rect.height);
}
