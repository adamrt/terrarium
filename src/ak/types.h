#pragma once

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

#define UNUSED(x) ((void)(x))

// This signifies that the variable is not owned by this scope.
#define TAKEN(x) ((x) = NULL)

#define BYTES_TO_KB(x) ((f64)(x) / 1024.0)
#define BYTES_TO_MB(x) ((f64)(x) / (1024.0 * 1024.0))
