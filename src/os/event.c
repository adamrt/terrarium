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

void os_mouse_position_from_event(const os_event_t* event, i32* out_mx, i32* out_my)
{
    ASSERT(event);

    switch (event->type) {
    case OS_EVENT_MOUSEMOVE:
        *out_mx = event->u.mousemove.pos_x;
        *out_my = event->u.mousemove.pos_y;
        break;
    case OS_EVENT_MOUSEBUTTON_DOWN:
        *out_mx = event->u.mousebutton.pos_x;
        *out_my = event->u.mousebutton.pos_y;
        break;
    case OS_EVENT_MOUSEBUTTON_UP:
        *out_mx = event->u.mousebutton.pos_x;
        *out_my = event->u.mousebutton.pos_y;
        break;
    case OS_EVENT_MOUSEWHEEL:
        *out_mx = event->u.mousewheel.pos_x;
        *out_my = event->u.mousewheel.pos_y;
        break;
    default:
        // -1 is invalid because the x,y should be within screen coordinates.
        *out_mx = -1;
        *out_my = -1;
        break;
    }
}
