#include <stdio.h>
#include <string.h>

#include "ak/types.h"
#include "gfx/color.h"
#include "gfx/draw.h"
#include "gfx/surface.h"
#include "os/display.h"
#include "os/event.h"
#include "os/os.h"

enum {
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
};

i32 main(i32 argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    os_init();
    os_display_t* display = os_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx_surface_t* surface = gfx_surface_create(SCREEN_WIDTH, SCREEN_HEIGHT);

    bool is_running = true;
    os_event_t event = { 0 };

    while (is_running) {
        while (os_event_poll(&event)) {
            switch (event.type) {
            case OS_EVENT_QUIT:
                is_running = false;
                break;
            case OS_EVENT_MOUSE_MOVE:
                break;
            case OS_EVENT_MOUSE_BUTTON:
                break;
            case OS_EVENT_UNKNOWN:
                break;
            }
        }

        gfx_surface_clear(surface, gfx_black);

        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            gfx_draw_pixel(surface, x, 200, gfx_red);
        }
        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            gfx_draw_pixel(surface, x, 300, gfx_green);
        }
        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            gfx_draw_pixel(surface, x, 400, gfx_blue);
        }

        os_display_present(display, surface);
    }

    gfx_surface_destroy(surface);
    os_display_destroy(display);
    os_shutdown();

    return 0;
}
