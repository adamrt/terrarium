TARGET = terrarium
CC ?= gcc

SDL_FLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS  := $(shell pkg-config --libs sdl2)

WARN_FLAGS  = -Wall -Wextra -Werror -Wshadow -Wundef -Wformat=2 -Wvla -Wconversion -Wdouble-promotion
DEBUG_FLAGS = -g3 -O0 -fstack-protector-strong -fno-omit-frame-pointer
SAN_FLAGS = -fsanitize=address,undefined

CFLAGS += -std=c11 -Iuserland -MMD -MP $(WARN_FLAGS) $(DEBUG_FLAGS) $(SAN_FLAGS) $(SDL_FLAGS)
LDFLAGS += $(SAN_FLAGS)
LDLIBS += -lm $(SDL_LIBS)

BUILD_DIR = build
SOURCES = $(shell find userland/ -name "*.c")
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
# LSAN defaults to disabled on MacOS due to how noisy it is.
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	@LSAN_OPTIONS=suppressions=meta/suppressions.txt:print_suppressions=0 \
	ASAN_OPTIONS=fast_unwind_on_malloc=0:symbolize=1 \
	./$<

.PHONY: debug
debug: $(BUILD_DIR)/$(TARGET)
	@lldb ./$<

.PHONY: fmt
fmt:
	@find userland -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +
