# CHIP-8 Emulator (SDL2)

A simple CHIP-8 interpreter with SDL2 rendering and keyboard input. Targets ~500 Hz CPU cycles and ~60 Hz delay/sound timers; can run one or multiple ROMs from the CLI.

## Usage
- Build
```sh
  make
```
- Run:
```sh
  ./build/chip8-emulator path_to_rom1 [path_to_rom2 ...]
```

## Features
- Full CHIP-8 opcode set (64×32 monochrome display).
- SDL2 renderer with scaled window and simple pixel buffer.
- Keyboard mapping to CHIP-8 hex keypad; Esc/close quits.
- Supports running multiple ROMs sequentially from command-line args.
- Basic logging for init/load errors.

## Requirements
- C compiler (tested with gcc, `-std=c2x`).
- SDL2 development headers/libraries (`sdl2-config` or `pkg-config sdl2`).
- Tested on Linux; SDL2 availability required on other platforms.

## Build
```sh
make            # builds to build/chip8-emulator
make clean      # remove build artifacts
