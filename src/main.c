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

    assert(SDL_Init(SDL_INIT_VIDEO) >= 0);

    SDL_Window* window = SDL_CreateWindow("Terrarium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    assert(window);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    assert(renderer);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    assert(texture);

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

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(u32));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
