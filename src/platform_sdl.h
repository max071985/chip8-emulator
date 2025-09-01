#include <SDL2/SDL.h>

#define WINDOW_TITLE "test window"
#define SCREEN_WIDTH 800
#define SCREEN_LENGTH 400
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 32

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} Platform;

bool plat_init(Platform *p);
bool plat_display_create(Platform *p);
bool plat_renderer_create(Platform *p);
bool plat_texture_create(Platform *p);
void plat_cleanup(Platform *p);
