TARGET = terrarium
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror `pkg-config --cflags sdl2` -Isrc
LDFLAGS = `pkg-config --libs sdl2`

BUILD_DIR = build
SOURCES = $(shell find src/ -name "*.c")
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Compile
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Run
.PHONY: run
run:
	./$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR): 
	mkdir -p $@/src
	mkdir -p $@/src/ak
	mkdir -p $@/src/os
	mkdir -p $@/src/gfx
