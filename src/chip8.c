#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include "logger.h"

/* chip8.c is responsible to handle the chip-8 VM-
    Initializes the vm and translates + executes opcodes
    - This is a legacy chip-8 standard */

static uint16_t fetch_instruction(Chip8* p);

static const uint8_t chip8_font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/* Initializes the VM with default values */
bool chip8_init(Chip8 *p)
{
    // Maybe first set all bits to 0 and then check start index? not sure if required- but will be safer
    *p = (Chip8){ .pc = CHIP8_PC_START_INDEX, .keys = {0} };

    // Load fontset into memory starting at 0x050
    for (int i = 0; i < 80; i++)
    {
        p->memory[FONT_BASE + i] = chip8_font[i];
    }


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

/* Fetches the next instruction from vm's memory at pc (program counter) and executes it, updates the vm values (p) accordingly.
Returns: false if a command is invalid, true otherwise */
bool chip8_cycle(Chip8 *p)
{
    // Fetch:
    uint16_t instruction = fetch_instruction(p);
    p->pc += 2; // Increment pc
    p->draw_flag = false;
    
    // Decode instruction:
    uint16_t NNN = instruction & 0x0FFF;
    uint8_t NN = instruction & 0x00FF;
    uint8_t N = instruction & 0x000F;
    uint8_t X = (instruction & 0x0F00) >> 8;
    uint8_t Y = (instruction & 0X00F0) >> 4;

    switch (instruction & 0xF000)
    {
        case 0x0000:
            switch (instruction)
            {
            case 0x00E0:
                for (int i = 0; i < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; i++)
                    p->display[i] = 0;
                p->draw_flag = true;
                break;
            case 0x00EE:
                if(p->sp == 0)
                {
                    log_msg(LOG_ERROR, "chip8-vm stack underflow at PC=%X", p->pc - 2);
                    return true;
                }
                p->sp--;
                p->pc = p->stack[p->sp];
                break;
            default:
                break;
            }
            break;
        case 0x1000:
            if (NNN >= CHIP8_MEM_SIZE || NNN < CHIP8_PC_START_INDEX)
            {
                log_msg(LOG_ERROR, "illegal jump address: NNN=%X provided at PC=%X", NNN, p->pc - 2);
                return true;
            }
            p->pc = NNN;
            break;
        case 0x2000:
            if (p->sp > CHIP8_STACK_SIZE - 1)
            {
                log_msg(LOG_ERROR, "memory stack overflow at PC=%X", p->pc - 2);
                return true;
            }
            p->stack[p->sp++] = p->pc;
            p->pc = NNN;
            break;
        case 0x3000:
            if (X < CHIP8_REGISTER_COUNT)
            {
                if (p->V[X] == NN)
                {
                    p->pc += 2;
                }
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0x4000:
            if (X < CHIP8_REGISTER_COUNT)
            {
                if (p->V[X] != NN)
                {
                    p->pc += 2;
                }
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0x5000:
            if ((instruction & 0x000F) != 0) 
            {
                log_msg(LOG_ERROR, "illegal opcode %X at PC=%X", instruction, p->pc-2);
                return true;
            }
            if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
            {
                if (p->V[X] == p->V[Y])
                {
                    p->pc += 2;
                }
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0x6000:
            if (X < CHIP8_REGISTER_COUNT)
            {
                p->V[X] = NN;
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0x7000:
            if (X < CHIP8_REGISTER_COUNT)
            {
                p->V[X] += NN;
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0x8000:
            switch (instruction & 0x000F)
            {
                case 0x0000:
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        p->V[X] = p->V[Y];
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                case 0x0001:
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        p->V[X] |= p->V[Y];
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                case 0x0002:
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        p->V[X] &= p->V[Y];
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                case 0x0003:
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        p->V[X] ^= p->V[Y];
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                case 0x0004: {
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        uint16_t sum = p->V[X] + p->V[Y];
                        p->V[0xF] = (sum > 0xFF) ? 1 : 0;
                        p->V[X] = (uint8_t)sum;
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    } }
                    break;
                case 0x0005: {
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        uint8_t borrow = p->V[X] >= p->V[Y];
                        p->V[0xF] = borrow ? 1 : 0;
                        p->V[X] = (uint8_t)(p->V[X] - p->V[Y]);
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    } }
                    break;
                case 0x0006:
                    if (X < CHIP8_REGISTER_COUNT)
                    {
                        //p->V[0xF] = p->V[X] & 0x01;
                        //p->V[X] >>= 1;
                        p->V[0xF] = p->V[X] & 0x01;
                        p->V[X] = p->V[X] >> 1;
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                case 0x0007: { 
                    if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
                    {
                        uint8_t borrow = (p->V[Y] >= p->V[X]);
                        p->V[0xF] = borrow ? 1 : 0;
                        p->V[X] = p->V[Y] - p->V[X];
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    } }
                    break;
                case 0x000E:
                    if (X < CHIP8_REGISTER_COUNT)
                    {
                        //p->V[0xF] = (p->V[X] >> 7) & 0x01;
                        //p->V[X] <<= 1;
                        p->V[0xF] = (p->V[X] >> 7) & 0x01;
                        p->V[X] = p->V[X] << 1;
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                        return true;
                    }
                    break;
                default:
                    log_msg(LOG_INFO, "Unknown opcode %X at PC=%X", instruction, p->pc - 2);
                    return true;
            }
            break;
        case 0x9000:
            if ((instruction & 0x000F) != 0) 
            {
                log_msg(LOG_ERROR, "illegal opcode %X at PC=%X", instruction, p->pc-2);
                return true;
            }
            if (X < CHIP8_REGISTER_COUNT && Y < CHIP8_REGISTER_COUNT)
            {
                if (p->V[X] != p->V[Y])
                    p->pc += 2;
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0xA000:
            p->I = NNN;
            break;
        case 0xB000:
            p->pc = p->V[0] + NNN;
            break;
        case 0xC000:
            if (X < CHIP8_REGISTER_COUNT)
            {
                p->V[X] = (rand() & 0xFF) & NN;
            }
            else
            {
                log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0xD000: {
            uint8_t x0 = p->V[X]; 
            uint8_t y0 = p->V[Y]; 
            
            uint8_t collision = 0;
            for (uint8_t row = 0; row < N; row++)
            {
                if (p->I + row >= CHIP8_MEM_SIZE) 
                {
                    log_msg(LOG_ERROR, "sprite read OOB at PC=%X", p->pc-2);
                    return true;
                }

                uint8_t sprite = p->memory[p->I + row];
                for (uint8_t col = 0; col < 8; col++)
                {
                    if (sprite & (0x80u >> col))
                    {
                        uint8_t x = (x0 + col) % CHIP8_DISPLAY_WIDTH;
                        uint8_t y = (y0 + row) % CHIP8_DISPLAY_HEIGHT;

                        int idx = y * CHIP8_DISPLAY_WIDTH + x;

                        uint8_t before = p->display[idx];
                        uint8_t after  = before ^ 1u;

                        if (before && !after) collision = 1;
                        p->display[idx] = after;
                    }
                }
            }
            p->V[0xF] = collision;
            p->draw_flag = true;
            }
            break;
        case 0xE000:
            switch (instruction & 0x00FF)
            {
            case 0x009E:
                if (p->keys[p->V[X] & 0x000F]) p->pc += 2;
                break;
            case 0x00A1:
                if (!p->keys[p->V[X] & 0x000F]) p->pc += 2;
                break;        
            default:
                log_msg(LOG_INFO, "Unknown opcode %X at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        case 0xF000:
            switch (instruction & 0x00FF)
            {
            case 0x0007:
                if (X < CHIP8_REGISTER_COUNT)
                {
                    p->V[X] = p->delay_timer;
                }
                else
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                break;
            case 0x000A:
                bool pressed = false;
                for (int i = 0; i < CHIP8_KEY_COUNT; i++)
                {
                    if (p->keys[i])
                    {
                        p->V[X] = i;
                        pressed = true;
                        break;
                    }
                }
                if (!pressed) // a key is not pressed - repeat command until it is.
                    p->pc -= 2;
                break;
            case 0x0015:
                if (X < CHIP8_REGISTER_COUNT)
                {
                    p->delay_timer = p->V[X];
                }
                else
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                break;
            case 0x0018:
                if (X < CHIP8_REGISTER_COUNT)
                {
                    p->sound_timer = p->V[X];
                }
                else
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                break;
            case 0x001E:
                if (X < CHIP8_REGISTER_COUNT)
                {
                    p->I += p->V[X];
                }
                else
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                break;
            case 0x0029:
                if (X < CHIP8_REGISTER_COUNT)
                {
                    p->I = FONT_BASE + (p->V[X] * 5);
                }
                else
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                break;
            case 0x0033:
                p->memory[p->I] = p->V[X] / 100;
                p->memory[p->I + 1] = (p->V[X] / 10) % 10;
                p->memory[p->I + 2] = p->V[X] % 10;
                break;
            case 0x0055:
                if (X >= CHIP8_REGISTER_COUNT)
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                for (int i = 0; i <= X; i++)
                    p->memory[p->I + i] = p->V[i];
                //p->I += X + 1; // LEGACY
                break;
            case 0x0065:
                if (X >= CHIP8_REGISTER_COUNT)
                {
                    log_msg(LOG_ERROR, "[instruciton: %i] register index out-of-range at PC=%X", instruction, p->pc - 2);
                    return true;
                }
                for (int i = 0; i <= X; i++)
                    p->V[i] = p->memory[p->I + i];
                //p->I += X + 1; // LEGACY
                break;
            default:
                log_msg(LOG_INFO, "Unknown opcode %X at PC=%X", instruction, p->pc - 2);
                return true;
            }
            break;
        default:
            log_msg(LOG_INFO, "Unknown opcode %X at PC=%X", instruction, p->pc - 2);
            return true;
    }


    return false;
}

/* Returns a combined number with pc and pc+1 instuctions */
static uint16_t fetch_instruction(Chip8 *p)
{
    if(p->pc > CHIP8_MEM_SIZE - 1)
    {
        log_msg(LOG_ERROR, "trying to fetch out-of-memory commands");
        return 0;
    }
    uint8_t top = p->memory[p->pc];
    uint8_t bot = p->memory[p->pc + 1];
    return (uint16_t)((top<<8) | bot);
}
