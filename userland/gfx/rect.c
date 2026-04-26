// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "gfx/gfx.h"

bool gfx_rect_contains(gfx_rect_t rect, i32 x, i32 y)
{
    return (x >= rect.x && x < rect.x + rect.width) && (y >= rect.y && y < rect.y + rect.height);
}
