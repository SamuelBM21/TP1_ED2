# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -std=c11

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output executable
TARGET := $ pesquisa

# Default rule
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@

# Compiling source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean
