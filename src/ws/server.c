#include "ws/server.h"
#include "ak/assert.h"
#include "ak/types.h"
#include "gfx/color.h"
#include "gfx/draw.h"
#include "gfx/surface.h"
#include "os/display.h"

struct ws_server {
    i32 width, height;
    gfx_color_t background;

    os_display_t* display;
    gfx_surface_t* desktop;
};

ws_server_t* ws_server_create(mem_allocator_t* alloc, i32 width, i32 height)
{
    ASSERT(alloc);
    ASSERT(width > 0);
    ASSERT(height > 0);

    ws_server_t* server = mem_alloc(alloc, sizeof(*server));
    ASSERT(server);

    os_display_t* display = os_display_create(alloc, width, height);
    ASSERT(display);

    gfx_surface_t* desktop = gfx_surface_create(alloc, width, height);
    ASSERT(desktop);


    server->width = width;
    server->height = height;
    server->background = gfx_black;

    server->display = display;
    server->desktop = desktop;

    return server;
}

void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server)
{
    ASSERT(alloc);
    ASSERT(server);

    os_display_destroy(alloc, server->display);
    gfx_surface_destroy(alloc, server->desktop);
    mem_free(alloc, server);
}

void ws_server_render(ws_server_t* server)
{
    gfx_surface_clear(server->desktop, server->background);
    {
        for (i32 x = 0; x < server->width; ++x) {
            gfx_draw_pixel(server->desktop, x, 200, gfx_red);
        }
        for (i32 x = 0; x < server->width; ++x) {
            gfx_draw_pixel(server->desktop, x, 300, gfx_green);
        }
        for (i32 x = 0; x < server->width; ++x) {
            gfx_draw_pixel(server->desktop, x, 400, gfx_blue);
        }
    }
    os_display_present(server->display, server->desktop);
}
