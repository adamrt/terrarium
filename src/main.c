#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "ak/types.h"
#include "os/os.h"

enum {
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
};

#define BLACK (0x000000FF)
#define WHITE (0xFFFFFFFF)
#define RED (0xFF0000FF)
#define GREEN (0x00FF00FF)
#define BLUE (0x0000FFFF)

u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

void clear_pixels(u32 color) {
    for (i32 y = 0; y < SCREEN_HEIGHT; ++y) {
        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[y * SCREEN_WIDTH + x] = color;
        }
    }
}

void draw_pixel(u32 x, u32 y, u32 color) {
    pixels[y * SCREEN_WIDTH + x] = color;
}

i32 main(i32 argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    os_init();

    os_display_t* display = os_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);

    bool is_running = true;
    SDL_Event event = { 0 };

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
                break;
            }
        }

        clear_pixels(BLACK);

        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[200 * SCREEN_WIDTH + x] = RED;
        }
        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[300 * SCREEN_WIDTH + x] = GREEN;
        }
        for (i32 x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[400 * SCREEN_WIDTH + x] = BLUE;
        }

        os_display_present(display, pixels);
    }

    os_shutdown();

    return 0;
}
