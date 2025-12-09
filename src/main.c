#include <stdio.h>
#include "chip8.h"
#include "platform_sdl.h"
#include "logger.h"

/* Chip8 entry point
    Responsible for initializing the SDL, the VM and running the main command loop
    Program usage: ./chip8-emulator path_to_rom [path_to_rom_2] ... */

// Main loop + timer delay constants
#define TIMER_FREQ 16   // ~60 hz timer
#define CPU_FREQ 500.0  // CPU freq
#define CPU_CMDS 1000.0 // Commands per cycle

void main_cleanup(Platform *plat, Chip8 *vm);

int main(int argc, char *argv[]) {
    Platform plat = {0};
    Chip8 vm = {0};

    const double cpu_delay = CPU_CMDS / CPU_FREQ;   // The delay between each command the vm executes

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
            bool running = true;    // Keyboard interrupt flag
            uint64_t timer_last_tick = SDL_GetTicks();
            uint64_t cpu_last_tick = SDL_GetTicks();

            /*
                Keyboard input event loop - Checks for early interrupts and updates vm's key[] array if a key is pressed
                    - Press "ESC" to terminate the program early
                    - Chip8 keyboard abides by the:
                            1 2 3 4 ->  1 2 3 C
                            q w e r ->  4 5 6 D
                            a s d f ->  7 8 9 E
                            z x c v ->  A 0 B F
                      layout. the keys on the left are mapped to the keys in the standard chip8 keyboard (right). 
            */
            while (running)
            {
                SDL_Event e;
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT) running = false;
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;

                    int key = map_key(e.key.keysym.sym);
                    if (key != -1)
                    {
                        if (e.type == SDL_KEYDOWN)   vm.keys[key] = 1;
                        if (e.type == SDL_KEYUP)     vm.keys[key] = 0;
                    }
                }
                uint64_t current_tick = SDL_GetTicks();
                
                if (current_tick - cpu_last_tick >= cpu_delay)
                {
                    chip8_cycle(&vm);
                    cpu_last_tick = current_tick;
                }

                // Update timer ticks
                if (current_tick - timer_last_tick >= TIMER_FREQ)
                {
                    if (vm.delay_timer) vm.delay_timer--;
                    if (vm.sound_timer) vm.sound_timer--;
                    timer_last_tick = current_tick;
                }

                // Invokes render if the executed opcode triggers a render request (draw_flag is true)
                if (vm.draw_flag)
                    plat_render(&plat, &vm);
                
            }
        }
    }
    main_cleanup(&plat, &vm); // Cleanup before termination
    return 0;
}

// Main cleanup function
void main_cleanup(Platform *plat, Chip8 *vm)
{
    if (plat)
        plat_cleanup(plat);
}