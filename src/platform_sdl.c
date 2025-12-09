#include "platform_sdl.h"
#include "logger.h"

/*
    responsible for the SDL2 platform operations:
    - Initializes and builds the window
    - Renders VM states
    - Converts user input to chip-8 standard
*/

bool plat_init(Platform *p)
{
    bool flag = false;
    // Initialize the platform system
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        log_msg(LOG_ERROR, "Failed to init SDL: %s", SDL_GetError());
        return true;
    }

    // Ensure clean initial state
    p->window = NULL;
    p->renderer = NULL;
    p->texture = NULL;

    flag |= plat_display_create(p);      // Create the window object
    flag |= plat_renderer_create(p);     // Create the renderer object
    flag |= plat_texture_create(p);      // Create the texture object

    SDL_SetRenderDrawColor(p->renderer, 0, 0, 0, 255);  // Default color scheme
    SDL_RenderClear(p->renderer);
    if (flag)
    {
        plat_cleanup(p);
        return true;
    }
    return false;
}

bool plat_display_create(Platform *p)
{
    p->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!p->window)
    {
        log_msg(LOG_ERROR, "Failed to create the display window: %s", SDL_GetError());
        return true;
    }
    return false;
}

bool plat_renderer_create(Platform *p)
{
    p->renderer = SDL_CreateRenderer(p->window, -1, SDL_RENDERER_ACCELERATED);
    if (!p->renderer)
    {
        log_msg(LOG_ERROR, "Failed to create the renderer: %s", SDL_GetError());
        return true;
    }
    SDL_RenderSetLogicalSize(p->renderer, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    return false;
}

bool plat_texture_create(Platform *p)
{
    p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    if (!p->texture)
    {
        log_msg(LOG_ERROR, "Failed to create the texture: %s", SDL_GetError());
        return true;
    }
    return false;
}

void plat_cleanup(Platform *p)
{
    if (p->texture) {
        SDL_DestroyTexture(p->texture);
        p->texture = NULL;
    }
    if (p->renderer) {
        SDL_DestroyRenderer(p->renderer);
        p->renderer = NULL;
    }
    if (p->window) {
        SDL_DestroyWindow(p->window);
        p->window = NULL;
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}

bool plat_display_clear(Platform *p)
{
    if (!p->renderer)
    {
        return true;
    }
    SDL_RenderClear(p->renderer);
    SDL_RenderPresent(p->renderer);
    return false;
}

bool plat_render(Platform *p, Chip8* vm)
{
    for (int i = 0; i < CHIP8_DISPLAY_HEIGHT*CHIP8_DISPLAY_WIDTH; i++)
        p->pixels[i] = vm->display[i] ? 0xFFFFFFFFu : 0x000000FFu;
    SDL_UpdateTexture(p->texture, NULL, p->pixels, CHIP8_DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(p->renderer);
    SDL_RenderCopy(p->renderer, p->texture, NULL, NULL);
    SDL_RenderPresent(p->renderer);
    return false;
}

/* Maps the layouts:
    1 2 3 4 ->  1 2 3 C
    q w e r ->  4 5 6 D
    a s d f ->  7 8 9 E
    z x c v ->  A 0 B F */
int map_key(SDL_Keycode k) {
    switch (k) 
    {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;
        
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;
        
        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;
    }

    return -1; // not a Chip-8 key
}
