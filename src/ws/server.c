#include "ak/ak.h"
#include "gfx/gfx.h"
#include "os/os.h"
#include "ws/ws.h"

#include <string.h>

enum {
    WS_SERVER_WINDOW_MAX = 3,
};

static inline ws_event_t event_from_os_event(const ws_window_t* window, const os_event_t* os_event)
{
    ws_event_t ws_event = { 0 };

    // Conver type
    ws_event.type = (ws_event_type_e)os_event->type;

    // Copy all data
    (void)memcpy(&ws_event.u, &os_event->u, sizeof(os_event->u));

    // Localize events to the window
    switch (ws_event.type) {
    case WS_EVENT_MOUSEMOVE:
        ws_event.u.mousemove.pos_x -= window->rect.x;
        ws_event.u.mousemove.pos_y -= window->rect.y;
        break;
    case WS_EVENT_MOUSEBUTTON_DOWN:
    case WS_EVENT_MOUSEBUTTON_UP:
        ws_event.u.mousebutton.pos_x -= window->rect.x;
        ws_event.u.mousebutton.pos_y -= window->rect.y;
        break;
    case WS_EVENT_MOUSEWHEEL:
        ws_event.u.mousewheel.pos_x -= window->rect.x;
        ws_event.u.mousewheel.pos_y -= window->rect.y;
        break;
    default:
        break;
    }

    return ws_event;
}

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

void ws_server_event_handle(ws_server_t* server, const os_event_t* os_event)
{
    ASSERT(server);

    // Loop and do hit testing
    ws_window_t* window = server->windows[0];

    ws_event_t ws_event = event_from_os_event(window, os_event);

    // FIXME: pass to the hit window
    (void)ws_event;
}
