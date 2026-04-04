#include "SDL_events.h"
#include "ak/assert.h"
#include "ak/types.h"
#include "gfx/surface.h"
#include "os/display.h"
#include "os/event.h"
#include "os/os.h"

#include <SDL2/SDL.h>

struct os_display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    i32 width, height;
};

void os_init(void)
{
    ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0);
}

void os_shutdown(void)
{
    SDL_Quit();
}

os_display_t* os_display_create(i32 width, i32 height)
{
    ASSERT(width > 0);
    ASSERT(height > 0);

    os_display_t* display = calloc(1, sizeof(*display));
    ASSERT(display);

    display->window = SDL_CreateWindow("Terrarium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    ASSERT(display->window);
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    ASSERT(display->renderer);
    display->texture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    ASSERT(display->texture);

    display->width = width;
    display->height = height;

    return display;
}

void os_display_destroy(os_display_t* display)
{
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    free(display);
}

void os_display_present(os_display_t* display, gfx_surface_t* surface)
{
    ASSERT(display->width == surface->width);
    ASSERT(display->height == surface->height);

    i32 pitch = surface->width * (i32)sizeof(u32);

    SDL_RenderClear(display->renderer);
    SDL_UpdateTexture(display->texture, NULL, surface->data, pitch);
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

bool os_event_poll(os_event_t* out)
{
    ASSERT(out);

    memset(out, 0, sizeof(*out));

    SDL_Event event = { 0 };
    if (SDL_PollEvent(&event) == 0) {
        return false;
    }

    switch (event.type) {
    case SDL_QUIT:
        out->type = OS_EVENT_QUIT;
        return true;
    case SDL_MOUSEMOTION:
        out->type = OS_EVENT_MOUSE_MOVE;
        out->u.mouse_move.x = event.motion.x;
        out->u.mouse_move.y = event.motion.y;
        return true;
    default:
        return false;
    }

    __builtin_unreachable();
}
