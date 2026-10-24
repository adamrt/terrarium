# This is GNUMake, requiring gmake on OpenBSD
UNAME_S := $(shell uname -s)

TARGET = terrarium

SDL_FLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS  := $(shell pkg-config --libs sdl3)

WARN_FLAGS  = -Wall -Wextra -Werror -Wshadow -Wundef -Wformat=2 -Wvla \
              -Wconversion -Wdouble-promotion -Wmissing-prototypes \
              -Wmissing-variable-declarations -Wimplicit-fallthrough \
              -Wstrict-prototypes -Wcast-qual -Wpointer-arith
DEBUG_FLAGS = -g3 -O1 -fstack-protector-strong -fno-omit-frame-pointer

# OpenBSD specifics
ifeq ($(UNAME_S),OpenBSD)
    SAN_FLAGS = -fsanitize=undefined -fsanitize-trap=all
    LDFLAGS += -Wl,--no-execute-only
else
    SAN_FLAGS = -fsanitize=address,undefined
endif

CFLAGS += -std=c11 -Isrc -MMD -MP $(WARN_FLAGS) $(DEBUG_FLAGS) $(SAN_FLAGS) $(SDL_FLAGS)
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
	@find src -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

.PHONY: check
check:
	@find src -type f \( -name "*.c" -o -name "*.h" \) -exec sh -c 'head -n 1 "$$1" | grep -q "Copyright" || (echo "missing license: $$1" && exit 1)' _ {} \;
	@find src -type f -name "*.h" -exec sh -c 'grep -q "^#pragma once$$" "$$1" || (echo "missing #pragma once: $$1" && exit 1)' _ {} \;
	@find src -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format --dry-run --Werror {} +
