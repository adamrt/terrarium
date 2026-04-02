#pragma once

#include "ak/types.h"
#include "gfx/surface.h"

typedef struct os_display os_display_t;

void os_init(void);
void os_shutdown(void);

os_display_t* os_display_create(i32 width, i32 height);
void os_display_destroy(os_display_t* display);

void os_display_present(os_display_t* display, gfx_surface_t* surface);
