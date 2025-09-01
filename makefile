CC        := gcc
CSTD      := c2x
CFLAGS    := -Wall -Wextra -std=$(CSTD) -g -MMD -MP

# Get SDL2 flags (prefer sdl2-config; fallback to pkg-config)
SDL_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL_LIBS   := $(shell sdl2-config --libs   2>/dev/null)
ifeq ($(strip $(SDL_CFLAGS)),)
  SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
  SDL_LIBS   := $(shell pkg-config --libs sdl2)
endif

CFLAGS    += $(SDL_CFLAGS)
LDFLAGS   := $(SDL_LIBS)

SRC_DIR   := src
BUILD_DIR := build
TARGET    := chip8-emulator

HDRS      := $(SRC_DIR)/chip8.h $(SRC_DIR)/logger.h $(SRC_DIR)/platform_sdl.h
SRCS      := $(SRC_DIR)/main.c $(SRC_DIR)/chip8.c $(SRC_DIR)/logger.c $(SRC_DIR)/platform_sdl.c
OBJS      := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS      := $(OBJS:.o=.d)

.PHONY: all clean
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
