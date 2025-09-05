#include <stdio.h>
#include "chip8.h"
#include "platform_sdl.h"
#include "logger.h"

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
            while (!chip8_cycle(&vm))
            {
                if (vm.draw_flag)
                    plat_render(&plat, &vm);
            }
        }
    }
    SDL_Delay(5000); 
    main_cleanup(&plat, &vm); // Quit
    return 0;
}

void main_cleanup(Platform *plat, Chip8 *vm)
{
    if (plat)
        plat_cleanup(plat);
}