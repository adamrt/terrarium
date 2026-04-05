#pragma once

#include "ak/mem.h"
#include "ak/types.h"
#include "gfx/surface.h"

typedef struct os_display os_display_t;

os_display_t* os_display_create(mem_allocator_t* alloc, i32 width, i32 height);
void os_display_destroy(mem_allocator_t* alloc, os_display_t* display);
void os_display_present(os_display_t* display, gfx_surface_t* surface);
