#include <stdio.h>
#include "chip8.h"
#include "platform_sdl.h"
#include "logger.h"

#define EMU_FREQUENCY 16 // ~60 hz timer

void main_cleanup(Platform *plat, Chip8 *vm);

int main(int argc, char *argv[]) {
    Platform plat = {0};
    Chip8 vm = {0};

    if (plat_init(&plat))   // Initialize the SDL2 platform
    {
        main_cleanup(&plat, &vm);
        exit(1);
    }

    if (argc < 2)
    {
        log_msg(LOG_ERROR, "Excepted at least 1 argument");
        main_cleanup(&plat, &vm);
        exit(1);
    }

    for (int i = 1; i < argc; i++) // Read arguments
    {
        char *path_to_file = argv[i];
        if (chip8_init(&vm))    // Initialize the chip8 emulator for this ROM
        {
            main_cleanup(&plat, &vm); // On failure, clear and exit
            exit(1);
        }
        if (!chip8_load_rom(&vm, path_to_file)) // Load the rom into the vm memory
        {
            bool running = true;
            uint64_t last_tick = SDL_GetTicks();
            while (!chip8_cycle(&vm) && running)
            {
                SDL_Event e;
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT) running = false;
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
                    // Reads input key and appends it to the vm's virtual keyboard
                    int key = map_key(e.key.keysym.sym);
                    if (key != -1)
                    {
                        if (e.type == SDL_KEYDOWN)   vm.keys[key] = 1;
                        if (e.type == SDL_KEYUP)     vm.keys[key] = 0;
                    }
                }
                if (vm.draw_flag)
                    plat_render(&plat, &vm);
                
                // Timer tick down
                uint64_t current_tick = SDL_GetTicks();
                if (current_tick - last_tick >= EMU_FREQUENCY)
                {
                    if (vm.delay_timer) vm.delay_timer--;
                    if (vm.sound_timer) vm.sound_timer--;
                    last_tick = current_tick;
                }
            }
        }
    }
    main_cleanup(&plat, &vm); // Quit
    return 0;
}

void main_cleanup(Platform *plat, Chip8 *vm)
{
    if (plat)
        plat_cleanup(plat);
}