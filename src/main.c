// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"
#include "exp/dummy/app.h"
#include "exp/logviewer/app.h"
#include "os/os.h"
#include "ws/ws.h"

#include <stdio.h>
#include <string.h>

enum {
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
};

i32 main(i32 argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    // Initialization
    os_init();
    rnd_init();

    mem_allocator_t* alloc = mem_debug_create();
    ws_server_t* server = ws_server_create(alloc, SCREEN_WIDTH, SCREEN_HEIGHT);

    ws_window_t* dummy_window = exp_dummy_create(alloc, 100, 150);
    ws_server_window_take(server, &dummy_window);

    ws_window_t* logviewer_window = exp_logviewer_create(alloc, 450, 150);
    ws_server_window_take(server, &logviewer_window);

    // Event Loop
    bool is_running = true;
    os_event_t event = { 0 };
    while (is_running) {
        while (os_event_poll(&event)) {
            if (event.type == OS_EVENT_QUIT || (event.type == OS_EVENT_KEY_DOWN && event.u.key.code == OS_KEY_ESCAPE)) {
                is_running = false;
                break;
            }
            ws_server_event_handle(alloc, server, &event);
        }

        ws_server_render(server);
    }

    // Shutdown
    ws_server_destroy(alloc, server);
    mem_shutdown(alloc);
    os_shutdown();

    return 0;
}
