#include "ak/types.h"
#include "gfx/surface.h"
#include "os/os.h"

#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

struct os_display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    i32 width, height;
};

void os_init(void) {
    assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
}

void os_shutdown(void) {
    SDL_Quit();
}

os_display_t* os_display_create(i32 width, i32 height) {
    assert(width > 0);
    assert(height > 0);

    os_display_t* display = calloc(1, sizeof(*display));
    assert(display);

    display->window = SDL_CreateWindow("Terrarium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    assert(display->window);
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    assert(display->renderer);
    display->texture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    assert(display->texture);

    display->width = width;
    display->height = height;

    return display;
}

void os_display_destroy(os_display_t* display) {
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    free(display);
}

void os_display_present(os_display_t* display, gfx_surface_t* surface) {
    assert(display->width == surface->width);
    assert(display->height == surface->height);

    SDL_RenderClear(display->renderer);
    SDL_UpdateTexture(display->texture, NULL, surface->data, surface->width * sizeof(u32));
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}
