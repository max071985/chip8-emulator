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

    flag |= plat_display_create(p);      // Create the window object
    flag |= plat_renderer_create(p);     // Create the renderer object
    flag |= plat_texture_create(p);      // Create the texture object

    if (flag)
    {
        // TODO: Cleanup
    }
    return false;
}

bool plat_display_create(Platform *p)
{
    p->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_LENGTH, 0);
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
