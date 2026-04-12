// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

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
    struct {
        ws_window_t* window;
        i32 dx, dy;
    } drag;
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
    server->drag.window = NULL;
    server->drag.dx = 0;
    server->drag.dy = 0;

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
    ASSERT(server);

    gfx_surface_clear(server->desktop, server->background);
    gfx_surface_draw_wallpaper(server->desktop, server->width, server->height);

    gfx_surface_blit(server->composited, server->desktop, 0, 0);

    gfx_color_t frame_color = gfx_color_rgb(100, 100, 100);

    for (i32 i = 0; i < server->window_count; ++i) {
        ws_window_t* window = server->windows[i];

        // Draw frame
        gfx_surface_fill_rect(server->composited, window->rect, frame_color);

        // Draw content
        window->func_draw(window);
        gfx_rect_t content_rect = ws_window_rect_content(window);
        gfx_surface_blit(server->composited, window->content, content_rect.x, content_rect.y);
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

ws_hit_t ws_server_window_hit_check(ws_server_t* server, i32 mx, i32 my)
{
    // FIXME: Probably shoudl reverse this order
    for (i32 i = 0; i < server->window_count; ++i) {
        ws_window_t* window = server->windows[i];
        gfx_rect_t content_rect = ws_window_rect_content(window);
        if (gfx_rect_contains(content_rect, mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_CONTENT };
        } else if (gfx_rect_contains(window->rect, mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_FRAME };
        }
    }

    return (ws_hit_t) { .window = NULL, .type = WS_HIT_NONE };
}

void ws_server_event_handle(ws_server_t* server, const os_event_t* os_event)
{
    ASSERT(server);

    if (os_event->type == OS_EVENT_MOUSEBUTTON_DOWN) {
        i32 mx = os_event->u.mousebutton.pos_x;
        i32 my = os_event->u.mousebutton.pos_y;
        ws_hit_t hit = ws_server_window_hit_check(server, mx, my);
        switch (hit.type) {
        case WS_HIT_NONE:
            ASSERT(hit.window == NULL);
            break;
        case WS_HIT_FRAME:
            ASSERT(hit.window);
            server->drag.window = hit.window;
            server->drag.dx = mx - hit.window->rect.x;
            server->drag.dy = my - hit.window->rect.y;
            break;
        case WS_HIT_CONTENT:
            ASSERT(hit.window);
            ws_event_t ws_event = event_from_os_event(hit.window, os_event);
            UNUSED(ws_event);
            break;
        }
        return;
    } else if (os_event->type == OS_EVENT_MOUSEBUTTON_UP) {
        server->drag.window = NULL;
        return;
    } else if (os_event->type == OS_EVENT_MOUSEMOVE) {
        if (server->drag.window) {
            server->drag.window->rect.x = os_event->u.mousemove.pos_x - server->drag.dx;
            server->drag.window->rect.y = os_event->u.mousemove.pos_y - server->drag.dy;
            return;
        }
    }

    // Loop and do hit testing
}
