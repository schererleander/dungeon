# --- Compiler Settings ---
CC ?= gcc
PKG_CONFIG ?= pkg-config

# Check if raylib is available via pkg-config
HAS_RAYLIB_PC := $(shell $(PKG_CONFIG) --exists raylib && echo yes)

ifeq ($(HAS_RAYLIB_PC),yes)
	RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib)
	RAYLIB_LIBS   := $(shell $(PKG_CONFIG) --libs raylib)
else
	# Fallback for systems without raylib pkg-config (using local lib/ and standard flags)
	RAYLIB_CFLAGS := 
	RAYLIB_LIBS   := -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath=./lib
endif

# User CFLAGS/LDFLAGS can be passed from environment
CFLAGS  += -Wall -Wextra -std=c99 -O2 -Iinclude $(RAYLIB_CFLAGS)
LDFLAGS += $(RAYLIB_LIBS) -lm

# --- Project Directories ---
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# --- Files ---
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# --- Output Binary ---
TARGET = $(BIN_DIR)/dungeon_game

# --- Build Rules ---
all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking $@"
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
