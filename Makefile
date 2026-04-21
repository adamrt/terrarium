TARGET = terrarium
CC ?= gcc

SDL_FLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS  := $(shell pkg-config --libs sdl2)

DEBUG_FLAGS = -g3 -O0 -Wall -Wextra -Werror -Wshadow -Wundef -Wformat=2 -Wvla -Wconversion -Wdouble-promotion -fstack-protector-strong
SAN_FLAGS = -fsanitize=address,undefined

CFLAGS += -std=c11 -Isrc -MMD -MP $(DEBUG_FLAGS) $(SAN_FLAGS) $(SDL_FLAGS)
LDFLAGS += $(SAN_FLAGS)
LDLIBS += -lm $(SDL_LIBS)

BUILD_DIR = build
SOURCES = $(shell find src/ -name "*.c")
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:%.o=%.d)

.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Compile
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Include header dependencies
-include $(DEPS)

# Clean
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

# Run
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	@./$<

.PHONY: debug
debug: $(BUILD_DIR)/$(TARGET)
	@lldb ./$<

.PHONY: fmt
fmt:
	@find src -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +
