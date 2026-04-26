// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"

static u32 s_state = 0;

// This is terrible entropy but we are avoiding using time.h
void rnd_init(void)
{
    s_state = (u32)(uintptr_t)&s_state;
}

// random_i32_range is a basic LCG using the same constants as GCC.
i32 rnd_i32_range(i32 min, i32 max)
{
    ASSERT(min <= max);
    if (min == max) {
        return min;
    }

    s_state = (1103515245 * s_state + 12345);
    u32 range = (u32)(max - min) + 1;

    return (i32)(s_state % range) + min;
}
