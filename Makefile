TARGET = terrarium
CC = clang

WARN_FLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wundef -Wformat=2 -Wvla -Wconversion
DEBUG_FLAGS = -g3 -O0 -fstack-protector-strong -fsanitize=address,undefined

CFLAGS = -std=c99 -Isrc -MMD -MP $(DEBUG_FLAGS) $(WARN_FLAGS) `pkg-config --cflags sdl2`
LDFLAGS = $(DEBUG_FLAGS) `pkg-config --libs sdl2`

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

.PHONY: fmt
fmt:
	find -X src -type f -name "*.c" -o -name "*.h" | xargs clang-format -i
