#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "constants.h"
#include "chip8.h"

#define WINDOW_TITLE "test window"
// Map platform sizes to core CHIP-8 display dimensions
#define SCREEN_WIDTH  CHIP8_DISPLAY_WIDTH
#define SCREEN_HEIGHT CHIP8_DISPLAY_HEIGHT
#define TEXTURE_WIDTH  CHIP8_DISPLAY_WIDTH
#define TEXTURE_HEIGHT CHIP8_DISPLAY_HEIGHT

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    int scale;
} Platform;

bool plat_init(Platform *p);
bool plat_display_create(Platform *p);
bool plat_renderer_create(Platform *p);
bool plat_texture_create(Platform *p);
void plat_cleanup(Platform *p);
bool plat_display_clear(Platform *p);
bool plat_render(Platform *p, Chip8* vm);

#endif
