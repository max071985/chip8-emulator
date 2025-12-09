#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

// Virtual machine and Chip8 properties based on the legacy chip8 standard
#define CHIP8_MEM_SIZE 4096
#define CHIP8_STACK_SIZE 16
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_PC_START_INDEX 0x200
#define CHIP8_KEY_COUNT 16
#define FONT_BASE 0x050

/* VM struct */
typedef struct {
    uint16_t pc;                        // Program counter
    uint16_t I;                         // 12 bit Register
    uint8_t V[CHIP8_REGISTER_COUNT];    // Register array
    uint8_t keys[CHIP8_KEY_COUNT];      // Key press status array (1 = pressed, 0 = not pressed)
    uint16_t stack[CHIP8_STACK_SIZE];   // Memory stack
    uint8_t sp;                         // Stack pointer
    uint8_t memory[CHIP8_MEM_SIZE];     // Memory array
    uint8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];    // Display pixels 2d array
    bool draw_flag;                     // render flag (1 = render, 0 = don't render)
    uint8_t delay_timer;                // delay timer
    uint8_t sound_timer;                // sound timer
} Chip8;

bool chip8_init(Chip8 *p);
bool chip8_load_rom(Chip8 *p, char *filename);
bool chip8_cycle(Chip8 *p);

#endif
