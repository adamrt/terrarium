#include "ak/assert.h"
#include "ak/types.h"
#include "gfx/surface.h"
#include "os/display.h"
#include "os/event.h"
#include "os/os.h"

#include <SDL2/SDL.h>

// Forward declarations
static os_key_code_e scancode_to_keycode(SDL_Scancode scancode);

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
        out->u.mouse_move.pos_x = event.motion.x;
        out->u.mouse_move.pos_y = event.motion.y;
        return true;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        out->type = OS_EVENT_MOUSE_BUTTON;
        out->u.mouse_button.pos_x = event.button.x;
        out->u.mouse_button.pos_y = event.button.y;
        out->u.mouse_button.is_pressed = event.button.state == SDL_PRESSED;

        switch (event.button.button) {
        case SDL_BUTTON_LEFT:
            out->u.mouse_button.button = OS_MOUSE_BUTTON_LEFT;
            break;
        case SDL_BUTTON_RIGHT:
            out->u.mouse_button.button = OS_MOUSE_BUTTON_RIGHT;
            break;
        case SDL_BUTTON_MIDDLE:
            out->u.mouse_button.button = OS_MOUSE_BUTTON_MIDDLE;
            break;
        default:
            ASSERT(false, "Unknown button: %d\n", event.button.button);
        }

        return true;

    case SDL_MOUSEWHEEL:
        out->type = OS_EVENT_MOUSE_WHEEL;
        out->u.mouse_wheel.pos_x = event.wheel.mouseX;
        out->u.mouse_wheel.pos_y = event.wheel.mouseY;
        out->u.mouse_wheel.scroll_x = event.wheel.preciseX;
        out->u.mouse_wheel.scroll_y = event.wheel.preciseY;
        return true;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
        out->type = OS_EVENT_KEY;
        out->u.key.is_pressed = event.key.state == SDL_PRESSED;
        out->u.key.is_repeat = event.key.repeat != 0;
        out->u.key.code = scancode_to_keycode(event.key.keysym.scancode);
        return true;

    default:
        return false;
    }

    __builtin_unreachable();
}

// All unsupported scancodes will return the 0 value (OS_KEY_UNKNOWN).
static os_key_code_e scancode_to_keycode(SDL_Scancode scancode)
{
    ASSERT(scancode >= 0, "Negative scancode: %d\n", scancode);
    ASSERT(scancode < SDL_NUM_SCANCODES, "Invalid scancode: %d\n", scancode);

    static const os_key_code_e table[SDL_NUM_SCANCODES] = {
        [SDL_SCANCODE_UNKNOWN] = OS_KEY_UNKNOWN,

        [SDL_SCANCODE_1] = OS_KEY_1,
        [SDL_SCANCODE_2] = OS_KEY_2,
        [SDL_SCANCODE_3] = OS_KEY_3,
        [SDL_SCANCODE_4] = OS_KEY_4,
        [SDL_SCANCODE_5] = OS_KEY_5,
        [SDL_SCANCODE_6] = OS_KEY_6,
        [SDL_SCANCODE_7] = OS_KEY_7,
        [SDL_SCANCODE_8] = OS_KEY_8,
        [SDL_SCANCODE_9] = OS_KEY_9,
        [SDL_SCANCODE_0] = OS_KEY_0,

        [SDL_SCANCODE_A] = OS_KEY_A,
        [SDL_SCANCODE_B] = OS_KEY_B,
        [SDL_SCANCODE_C] = OS_KEY_C,
        [SDL_SCANCODE_D] = OS_KEY_D,
        [SDL_SCANCODE_E] = OS_KEY_E,
        [SDL_SCANCODE_F] = OS_KEY_F,
        [SDL_SCANCODE_G] = OS_KEY_G,
        [SDL_SCANCODE_H] = OS_KEY_H,
        [SDL_SCANCODE_I] = OS_KEY_I,
        [SDL_SCANCODE_J] = OS_KEY_J,
        [SDL_SCANCODE_K] = OS_KEY_K,
        [SDL_SCANCODE_L] = OS_KEY_L,
        [SDL_SCANCODE_M] = OS_KEY_M,
        [SDL_SCANCODE_N] = OS_KEY_N,
        [SDL_SCANCODE_O] = OS_KEY_O,
        [SDL_SCANCODE_P] = OS_KEY_P,
        [SDL_SCANCODE_Q] = OS_KEY_Q,
        [SDL_SCANCODE_R] = OS_KEY_R,
        [SDL_SCANCODE_S] = OS_KEY_S,
        [SDL_SCANCODE_T] = OS_KEY_T,
        [SDL_SCANCODE_U] = OS_KEY_U,
        [SDL_SCANCODE_V] = OS_KEY_V,
        [SDL_SCANCODE_W] = OS_KEY_W,
        [SDL_SCANCODE_X] = OS_KEY_X,
        [SDL_SCANCODE_Y] = OS_KEY_Y,
        [SDL_SCANCODE_Z] = OS_KEY_Z,

        [SDL_SCANCODE_SPACE] = OS_KEY_SPACE,
        [SDL_SCANCODE_ESCAPE] = OS_KEY_ESCAPE,
        [SDL_SCANCODE_RETURN] = OS_KEY_ENTER,
        [SDL_SCANCODE_BACKSPACE] = OS_KEY_BACKSPACE,
        [SDL_SCANCODE_TAB] = OS_KEY_TAB,
        [SDL_SCANCODE_CAPSLOCK] = OS_KEY_CAPSLOCK,

        [SDL_SCANCODE_MINUS] = OS_KEY_MINUS,
        [SDL_SCANCODE_EQUALS] = OS_KEY_EQUALS,
        [SDL_SCANCODE_LEFTBRACKET] = OS_KEY_LEFTBRACKET,
        [SDL_SCANCODE_RIGHTBRACKET] = OS_KEY_RIGHTBRACKET,
        [SDL_SCANCODE_BACKSLASH] = OS_KEY_BACKSLASH,
        [SDL_SCANCODE_SEMICOLON] = OS_KEY_SEMICOLON,
        [SDL_SCANCODE_APOSTROPHE] = OS_KEY_APOSTROPHE,
        [SDL_SCANCODE_GRAVE] = OS_KEY_GRAVE,
        [SDL_SCANCODE_COMMA] = OS_KEY_COMMA,
        [SDL_SCANCODE_PERIOD] = OS_KEY_PERIOD,
        [SDL_SCANCODE_SLASH] = OS_KEY_FORWARDSLASH,

        [SDL_SCANCODE_LALT] = OS_KEY_LEFTALT,
        [SDL_SCANCODE_LCTRL] = OS_KEY_LEFTCONTROL,
        [SDL_SCANCODE_LSHIFT] = OS_KEY_LEFTSHIFT,
        [SDL_SCANCODE_LGUI] = OS_KEY_LEFTSUPER,
        [SDL_SCANCODE_RALT] = OS_KEY_RIGHTALT,
        [SDL_SCANCODE_RCTRL] = OS_KEY_RIGHTCONTROL,
        [SDL_SCANCODE_RSHIFT] = OS_KEY_RIGHTSHIFT,
        [SDL_SCANCODE_RGUI] = OS_KEY_RIGHTSUPER,

        [SDL_SCANCODE_UP] = OS_KEY_UP,
        [SDL_SCANCODE_DOWN] = OS_KEY_DOWN,
        [SDL_SCANCODE_LEFT] = OS_KEY_LEFT,
        [SDL_SCANCODE_RIGHT] = OS_KEY_RIGHT,
    };

    return table[scancode];
}
