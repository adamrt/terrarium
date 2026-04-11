#include "ak/ak.h"
#include "gfx/gfx.h"
#include "os/os.h"
#include "ws/ws.h"

enum {
    WS_SERVER_WINDOW_MAX = 3,
};

struct ws_server {
    i32 width, height;
    gfx_color_t background;

    os_display_t* display;
    gfx_surface_t* desktop;

    gfx_surface_t* composited;

    ws_window_t* windows[WS_SERVER_WINDOW_MAX];
    i32 window_count;
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

    gfx_surface_t* composited = gfx_surface_create(alloc, width, height);
    ASSERT(composited);

    server->width = width;
    server->height = height;
    server->background = gfx_black;

    server->display = display;
    server->desktop = desktop;
    server->composited = composited;

    server->window_count = 0;

    return server;
}

void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server)
{
    ASSERT(alloc);
    ASSERT(server);

    os_display_destroy(alloc, server->display);
    gfx_surface_destroy(alloc, server->desktop);
    gfx_surface_destroy(alloc, server->composited);

    for (i32 i = 0; i < server->window_count; ++i) {
        ws_window_t* window = server->windows[i];
        window->func_close(window);
    }

    mem_free(alloc, server);
}

void ws_server_render(ws_server_t* server)
{
    gfx_surface_clear(server->desktop, server->background);
    gfx_surface_draw_wallpaper(server->desktop, server->width, server->height);

    gfx_surface_blit(server->composited, server->desktop, 0, 0);

    for (i32 i = 0; i < server->window_count; ++i) {
        ws_window_t* window = server->windows[i];
        window->func_draw(window);
        gfx_surface_blit(server->composited, window->content, window->rect.x, window->rect.y);
    }

    os_display_present(server->display, server->composited);
}

void ws_server_window_take(ws_server_t* server, ws_window_t** window_take)
{
    ws_window_t* window = *window_take;
    *window_take = NULL;

    ASSERT(server->window_count + 1 < WS_SERVER_WINDOW_MAX);
    ASSERT(window->func_draw);
    ASSERT(window->func_close);

    server->windows[server->window_count++] = window;
}
