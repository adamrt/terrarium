// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "os/os.h"

static struct {
    // Modifier key active bitset. 0 = up, 1 = down
    u8 mod_bitset;
} g_event_state;

u8 os_mod_bitset(void)
{
    return g_event_state.mod_bitset;
}

void os_mod_bitset_set(os_mod_e mod, bool is_down)
{
    ASSERT(mod < OS_MOD__COUNT);
    ASSERT(mod > OS_MOD_UNKNOWN); // Protect -1 below
    bitset8_set(&g_event_state.mod_bitset, (u8)mod - 1, is_down);
}

os_mod_e os_key_to_mod(os_key_code_e code)
{
    ASSERT(code >= 0);
    ASSERT(code < OS_KEY__COUNT);

    static os_mod_e table[OS_KEY__COUNT] = {
        [OS_KEY_LEFTALT] = OS_MOD_ALT,
        [OS_KEY_RIGHTALT] = OS_MOD_ALT,
        [OS_KEY_LEFTCONTROL] = OS_MOD_CONTROL,
        [OS_KEY_RIGHTCONTROL] = OS_MOD_CONTROL,
        [OS_KEY_LEFTSHIFT] = OS_MOD_SHIFT,
        [OS_KEY_RIGHTSHIFT] = OS_MOD_SHIFT,
        [OS_KEY_LEFTSUPER] = OS_MOD_SUPER,
        [OS_KEY_RIGHTSUPER] = OS_MOD_SUPER,
    };

    return table[code];
}
