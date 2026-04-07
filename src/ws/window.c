#include "ws/window.h"
#include "ak/assert.h"
#include "ak/mem.h"
#include "gfx/color.h"
#include "gfx/surface.h"

ws_window_t* ws_window_create(mem_allocator_t* alloc, i32 x, i32 y, i32 width, i32 height)
{
    ASSERT(alloc);
    ASSERT(x >= 0)
    ASSERT(y >= 0)

    ws_window_t* window = mem_alloc(alloc, sizeof(*window));
    ASSERT(window);

    gfx_surface_t* content = gfx_surface_create(alloc, width, height);
    ASSERT(content);

    window->rect.x = x;
    window->rect.y = y;
    window->rect.width = width;
    window->rect.height = height;
    window->content_color = gfx_green;

    window->content = content;

    return window;
}

void ws_window_destroy(mem_allocator_t* alloc, ws_window_t* window)
{
    ASSERT(alloc);
    ASSERT(window);

    gfx_surface_destroy(alloc, window->content);
    mem_free(alloc, window);
}
