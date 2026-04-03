TARGET = terrarium
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror `pkg-config --cflags sdl2` -Isrc -MMD -MP
LDFLAGS = `pkg-config --libs sdl2`

BUILD_DIR = build
SOURCES = $(shell find src/ -name "*.c")
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:%.o=%.d)

.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Compile
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

# Include header dependencies
-include $(DEPS)

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Run
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	./$<
