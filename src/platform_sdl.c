#include "platform_sdl.h"
#include "logger.h"

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