#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "constants.h"
#include "chip8.h"

// SDL Platform constants

#define WINDOW_TITLE "Chip-8 emulator by DekZ"
#define WINDOW_SCALE 10
#define SCREEN_WIDTH  (CHIP8_DISPLAY_WIDTH * WINDOW_SCALE)
#define SCREEN_HEIGHT (CHIP8_DISPLAY_HEIGHT * WINDOW_SCALE)
#define TEXTURE_WIDTH  CHIP8_DISPLAY_WIDTH
#define TEXTURE_HEIGHT CHIP8_DISPLAY_HEIGHT

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixels[TEXTURE_WIDTH * TEXTURE_HEIGHT];
    int scale;
} Platform;

// Initialize SDL platform
bool plat_init(Platform *p);
bool plat_display_create(Platform *p);
bool plat_renderer_create(Platform *p);
bool plat_texture_create(Platform *p);

// SDL platform cleanup
void plat_cleanup(Platform *p);

// Clears the current display
bool plat_display_clear(Platform *p);

// Renders the framebuffer from the vm
bool plat_render(Platform *p, Chip8* vm);

// Maps the keys 1,2,3,4,q,w,e,r... into their chip8 keyboard counterparts (1->0, 2->1 etc.)
int map_key(SDL_Keycode k);

#endif
