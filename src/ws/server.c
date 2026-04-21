// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "exp/dummy/app.h"
#include "gfx/gfx.h"
#include "os/os.h"
#include "ws/ws.h"
#include "ws/ws_internal.h"

#include <string.h>

enum {
    WS_SERVER_WINDOW_MAX = 10,
    WS_SERVER_MENUBAR_SIZE = 20,
    WS_SERVER_BUTTON_PADDING = 6,
};

typedef enum {
    WS_DRAG_NONE,
    WS_DRAG_MOVE,
    WS_DRAG_RESIZE,
} ws_drag_type_e;

static const char* new_window_text = "New Window";

// Forward declarations
static ws_event_t ws_event_from_os_event(const ws_window_t* window, const os_event_t* os_event);
static void ws_server_window_close(mem_allocator_t* alloc, ws_server_t* server, ws_window_t* window);

struct ws_server {
    i32 width, height;
    gfx_color_t background;

    os_display_t* display;
    gfx_surface_t* desktop;
    gfx_surface_t* composited;

    ws_window_t* windows[WS_SERVER_WINDOW_MAX];
    i32 window_count;
    struct {
        ws_drag_type_e type;
        ws_window_t* window;
        i32 mouse_start_x, mouse_start_y;
        gfx_rect_t rect_start;
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
    server->background = gfx_color_rgb(82, 151, 153);

    server->display = display;
    server->desktop = desktop;
    server->composited = composited;

    server->window_count = 0;
    server->drag.window = NULL;
    server->drag.mouse_start_x = 0;
    server->drag.mouse_start_y = 0;
    server->drag.rect_start = (gfx_rect_t) { 0 };

    return server;
}

void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server)
{
    ASSERT(alloc);
    ASSERT(server);

    os_display_destroy(alloc, server->display);
    gfx_surface_destroy(alloc, server->desktop);
    gfx_surface_destroy(alloc, server->composited);

    i32 window_count = server->window_count;
    for (i32 i = 0; i < window_count; ++i) {
        ws_window_t* window = server->windows[i];
        ws_server_window_close(alloc, server, window);
    }

    mem_free(alloc, server);
}

static void ws_server_desktop_draw(ws_server_t* server)
{
    gfx_surface_fill(server->desktop, server->background);
    gfx_surface_blit(server->composited, server->desktop, 0, 0);
}

static void ws_server_window_draw(ws_server_t* server, ws_window_t* window)
{
    gfx_color_t frame_color = gfx_white;
    gfx_color_t border_color = gfx_black;
    gfx_color_t titlebar_text_color = gfx_black;

    // Draw frame
    gfx_surface_draw_rect(server->composited, ws_window_rect_total(window), border_color);
    gfx_surface_fill_rect(server->composited, ws_window_rect_frame(window), frame_color);

    // Titlebar
    gfx_rect_t tb = ws_window_rect_titlebar(window);
    gfx_surface_fill_rect(server->composited, tb, frame_color);
    i32 title_len = (i32)strlen(window->title);
    gfx_surface_draw_text(server->composited, tb.x + tb.width / 2 - title_len * GFX_FONT_WIDTH / 2, tb.y + 4, window->title, titlebar_text_color);

    // Close button
    gfx_surface_draw_rect(server->composited, ws_window_rect_button_close(window), border_color);

    // Maximize button
    gfx_rect_t max_rect = ws_window_rect_button_maximize(window);
    gfx_surface_draw_rect(server->composited, max_rect, border_color);
    i32 half = max_rect.width - WS_FRAME_BUTTON_SIZE / 2;
    gfx_surface_draw_rect(server->composited, (gfx_rect_t) { max_rect.x, max_rect.y, half, half }, border_color);

    // Draw content
    gfx_surface_draw_rect(server->composited, ws_window_rect_content_border(window), border_color);
    gfx_rect_t content_rect = ws_window_rect_content(window);
    gfx_surface_blit(server->composited, window->content, content_rect.x, content_rect.y);

    // Draw resize handle on top of content
    gfx_rect_t handle_rect = ws_window_rect_handle_resize(window);
    gfx_surface_fill_rect(server->composited, handle_rect, frame_color);
    gfx_surface_draw_line_h(server->composited, handle_rect.x, handle_rect.y, handle_rect.width - WS_FRAME_PADDING_SIZE, border_color);
    gfx_surface_draw_line_v(server->composited, handle_rect.x, handle_rect.y, handle_rect.height - WS_FRAME_PADDING_SIZE, border_color);
}

static gfx_rect_t ws_server_rect_new_window_button(void)
{
    i32 len = (i32)strlen(new_window_text);
    return (gfx_rect_t) {
        .x = 0,
        .y = 0,
        .width = GFX_FONT_WIDTH * len + WS_SERVER_BUTTON_PADDING * 2,
        .height = GFX_FONT_HEIGHT + WS_SERVER_BUTTON_PADDING * 2,
    };
}

void ws_server_render(ws_server_t* server)
{
    ASSERT(server);

    ws_server_desktop_draw(server);

    gfx_rect_t rect_new_window_button = ws_server_rect_new_window_button();
    gfx_surface_fill_rect(server->composited, rect_new_window_button, gfx_color_rgb(200, 200, 200));
    gfx_surface_draw_text(server->composited, rect_new_window_button.x + WS_SERVER_BUTTON_PADDING, rect_new_window_button.y + WS_SERVER_BUTTON_PADDING, new_window_text, gfx_black);

    for (i32 i = 0; i < server->window_count; ++i) {
        ws_window_t* window = server->windows[i];
        window->func_draw(window);
        ws_server_window_draw(server, window);
    }

    os_display_present(server->display, server->composited);
}

// FIXME: Is there a more elegant way to implement this
static void ws_server_window_to_front(ws_server_t* server, ws_window_t* window)
{
    for (i32 i = 0; i < server->window_count; ++i) {
        // Find the window in array
        if (window == server->windows[i]) {
            // Move all the remaining windows back 1 array position
            for (; i + 1 < server->window_count; ++i) {
                server->windows[i] = server->windows[i + 1];
            }
            // Place the window in the front
            server->windows[server->window_count - 1] = window;
            break;
        }
    }
}

void ws_server_window_take(ws_server_t* server, ws_window_t** window_take)
{
    ws_window_t* window = *window_take;
    *window_take = NULL;

    ASSERT(server->window_count < WS_SERVER_WINDOW_MAX);
    ASSERT(window->func_draw);
    ASSERT(window->func_close);

    server->windows[server->window_count++] = window;
}

static ws_hit_t ws_server_window_hit_check(ws_server_t* server, i32 mx, i32 my)
{
    for (i32 i = server->window_count - 1; i >= 0; --i) {
        ws_window_t* window = server->windows[i];

        if (gfx_rect_contains(ws_window_rect_handle_resize(window), mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_RESIZE };
        } else if (gfx_rect_contains(ws_window_rect_button_close(window), mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_CLOSE };
        } else if (gfx_rect_contains(ws_window_rect_button_maximize(window), mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_MAXIMIZE };
        } else if (gfx_rect_contains(ws_window_rect_content(window), mx, my)) {
            return (ws_hit_t) { .window = window, .type = WS_HIT_CONTENT };
        } else if (gfx_rect_contains(ws_window_rect_total(window), mx, my)) {
            // Use rect_total to include border
            return (ws_hit_t) { .window = window, .type = WS_HIT_FRAME };
        }
    }

    if (gfx_rect_contains(ws_server_rect_new_window_button(), mx, my)) {
        return (ws_hit_t) { .window = NULL, .type = WS_HIT_NEW_WIN };
    }

    return (ws_hit_t) { .window = NULL, .type = WS_HIT_NONE };
}

static void ws_server_window_close(mem_allocator_t* alloc, ws_server_t* server, ws_window_t* window)
{
    ASSERT(server);
    ASSERT(window);

    ws_window_close(alloc, window);
    server->window_count--;
}

static void ws_server_window_maximize_toggle(mem_allocator_t* alloc, ws_server_t* server, ws_window_t* window)
{
    ASSERT(server);
    ASSERT(window);

    if (!window->is_maximized) {
        ASSERT(window->restore_rect.width == 0);
        ASSERT(window->restore_rect.height == 0);

        window->is_maximized = true;
        window->restore_rect = window->rect;
        window->rect.x = 0;
        window->rect.y = WS_SERVER_MENUBAR_SIZE;
        ws_window_resize(alloc, window, server->width, server->height - WS_SERVER_MENUBAR_SIZE);
    } else {
        ASSERT(window->restore_rect.width != 0);
        ASSERT(window->restore_rect.height != 0);

        gfx_rect_t rect = window->restore_rect;

        window->is_maximized = false;
        window->restore_rect = (gfx_rect_t) { 0 };
        window->rect.x = rect.x;
        window->rect.y = rect.y;
        ws_window_resize(alloc, window, rect.width, rect.height);
    }
}

void ws_server_event_handle(mem_allocator_t* alloc, ws_server_t* server, const os_event_t* os_event)
{
    ASSERT(server);

    if (os_event->type == OS_EVENT_MOUSEBUTTON_DOWN) {
        i32 mx = os_event->u.mousebutton.pos_x;
        i32 my = os_event->u.mousebutton.pos_y;

        ws_hit_t hit = ws_server_window_hit_check(server, mx, my);
        if (hit.window != NULL) {
            ws_server_window_to_front(server, hit.window);
        }

        switch (hit.type) {
        case WS_HIT_NONE:
            ASSERT(hit.window == NULL);
            break;
        case WS_HIT_FRAME:
            ASSERT(hit.window);
            server->drag.type = WS_DRAG_MOVE;
            server->drag.window = hit.window;
            server->drag.mouse_start_x = mx;
            server->drag.mouse_start_y = my;
            server->drag.rect_start = hit.window->rect;
            break;
        case WS_HIT_CLOSE:
            ASSERT(hit.window);
            // FIXME: This should actually happen on mousebutton up
            ws_server_window_close(alloc, server, hit.window);
            break;
        case WS_HIT_MAXIMIZE:
            ASSERT(hit.window);
            // FIXME: This should actually happen on mousebutton up
            ws_server_window_maximize_toggle(alloc, server, hit.window);
            break;
        case WS_HIT_RESIZE:
            ASSERT(hit.window);
            server->drag.type = WS_DRAG_RESIZE;
            server->drag.window = hit.window;
            server->drag.mouse_start_x = mx;
            server->drag.mouse_start_y = my;
            server->drag.rect_start = hit.window->rect;
            break;
        case WS_HIT_CONTENT:
            ASSERT(hit.window);
            ws_event_t ws_event = ws_event_from_os_event(hit.window, os_event);
            UNUSED(ws_event);
            break;
        case WS_HIT_NEW_WIN: {
            ASSERT(!hit.window);
            ws_window_t* window = exp_dummy_create(alloc, rnd_i32_range(0, 700), rnd_i32_range(0, 500));
            ws_server_window_take(server, &window);
            break;
        }
        }
        return;
    } else if (os_event->type == OS_EVENT_MOUSEBUTTON_UP) {
        server->drag.type = WS_DRAG_NONE;
        server->drag.window = NULL;
        server->drag.mouse_start_x = 0;
        server->drag.mouse_start_y = 0;
        server->drag.rect_start = (gfx_rect_t) { 0 };
        return;
    } else if (os_event->type == OS_EVENT_MOUSEMOVE) {
        if (server->drag.window) {
            ASSERT(server->drag.type != WS_DRAG_NONE);

            ws_window_t* window = server->drag.window;
            i32 mx = os_event->u.mousemove.pos_x;
            i32 my = os_event->u.mousemove.pos_y;

            switch (server->drag.type) {
            case WS_DRAG_NONE:
                __builtin_unreachable();
            case WS_DRAG_MOVE: {
                i32 delta_x = mx - server->drag.mouse_start_x;
                i32 delta_y = my - server->drag.mouse_start_y;

                i32 new_x = server->drag.rect_start.x + delta_x;
                i32 new_y = server->drag.rect_start.y + delta_y;

                ws_window_move(window, new_x, new_y);
                break;
            }
            case WS_DRAG_RESIZE: {
                i32 delta_x = mx - server->drag.mouse_start_x;
                i32 delta_y = my - server->drag.mouse_start_y;

                i32 new_width = server->drag.rect_start.width + delta_x;
                i32 new_height = server->drag.rect_start.height + delta_y;

                ws_window_resize(alloc, window, new_width, new_height);
                break;
            }
            default:
                __builtin_unreachable();
            }
            return;
        }
    }
}

static ws_event_t ws_event_from_os_event(const ws_window_t* window, const os_event_t* os_event)
{
    ws_event_t ws_event = { 0 };

    // Convert type
    ws_event.type = (ws_event_type_e)os_event->type;

    // Copy all data
    (void)memcpy(&ws_event.u, &os_event->u, sizeof(os_event->u));

    // Localize events to the window
    // FIXME: This needs to be offset to the content rect, not window
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
