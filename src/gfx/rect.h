#pragma once

#include "ak/types.h"

typedef struct {
    i32 x, y;
    i32 width, height;
} gfx_rect_t;

bool gfx_rect_contains(gfx_rect_t rect, i32 x, i32 y);
