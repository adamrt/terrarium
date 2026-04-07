#pragma once

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

//
// Types
//

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

// i32
static inline i32 i32_min(i32 a, i32 b) { return (a < b) ? a : b; }
static inline i32 i32_max(i32 a, i32 b) { return (a > b) ? a : b; }
static inline i32 i32_clamp(i32 value, i32 lo, i32 hi) { return i32_min(i32_max(value, lo), hi); }

//
// Memory
//

typedef struct mem_allocator {
    void* (*func_alloc)(void* ctx, size_t size, const char* file, i32 line);
    void* (*func_realloc)(void* ctx, void* ptr, size_t new_size, const char* file, int line);
    void (*func_free)(void* ctx, void* ptr);
    void (*func_shutdown)(struct mem_allocator* self);

    void* ctx;
} mem_allocator_t;

mem_allocator_t* mem_heap_create(void);
mem_allocator_t* mem_debug_create(void);

#define mem_alloc(a, size)        ((a)->func_alloc((a)->ctx, size, __FILE__, __LINE__))
#define mem_realloc(a, ptr, size) ((a)->func_realloc((a)->ctx, (ptr), (size_t)(size), __FILE__, __LINE__))
#define mem_free(a, ptr)          ((a)->func_free((a)->ctx, ptr))
#define mem_shutdown(a)           ((a)->func_shutdown((a)))

//
// Bitset
//

void bitset8_set(u8* bitset, u8 position, bool value);
bool bitset8_get(u8 bitset, u8 position);

void bitset64_set(u64* bitset, u8 position, bool value);
bool bitset64_get(u64 bitset, u8 position);

//
// Macros
//

#define UNUSED(x) ((void)(x))
// This signifies that the variable is not owned by this scope.
#define TAKEN(x) ((x) = NULL)

#define BYTES_TO_KB(x) ((f64)(x) / 1024.0)
#define BYTES_TO_MB(x) ((f64)(x) / (1024.0 * 1024.0))

#define ASSERT(cond, ...)                                                                                      \
    do {                                                                                                       \
        if (!(cond)) {                                                                                         \
            fprintf(stderr, "Assertion failed: " __VA_ARGS__);                                                 \
            fprintf(stderr, " (%s) function %s, file %s, line %d\n", #cond, __FUNCTION__, __FILE__, __LINE__); \
            __builtin_trap();                                                                                  \
        }                                                                                                      \
    } while (0);
