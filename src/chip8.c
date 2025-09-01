#include "chip8.h"
#include "logger.h"
#include <stdio.h>

bool chip8_init(Chip8 *p)
{
    *p = (Chip8){ .pc = CHIP8_PC_START_INDEX };
    return false;
}

bool chip8_load_rom(Chip8 *p, char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        log_msg(LOG_ERROR, "Couldn't open file: '%s'", filename);
        return true;
    }
    fread(p->memory + CHIP8_PC_START_INDEX, 1, CHIP8_MEM_SIZE - CHIP8_PC_START_INDEX, f);    // Reads the rom bytes into the vm instance memory
    fclose(f);
    return false;
}

bool chip8_cycle(Chip8 *p)
{
    // TODO: Implement cycle loop
    // fetch/decode/execute + update timers
    return false;
}
