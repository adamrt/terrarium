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

enum {
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
};

#define BLACK (0x000000FF)
#define WHITE (0xFFFFFFFF)
#define RED (0xFF0000FF)
#define GREEN (0x00FF00FF)
#define BLUE (0x0000FFFF)

uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

void clear_pixels(uint32_t color) {
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[y * SCREEN_WIDTH + x] = color;
        }
    }
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    pixels[y * SCREEN_WIDTH + x] = color;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

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

        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[200 * SCREEN_WIDTH + x] = RED;
        }
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[300 * SCREEN_WIDTH + x] = GREEN;
        }
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            pixels[400 * SCREEN_WIDTH + x] = BLUE;
        }

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
