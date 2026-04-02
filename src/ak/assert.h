#pragma once

#include <stdio.h>

#define ASSERT(cond, ...)                                                                                      \
    do {                                                                                                       \
        if (!(cond)) {                                                                                         \
            fprintf(stderr, "Assertion failed: " __VA_ARGS__);                                                 \
            fprintf(stderr, " (%s) function %s, file %s, line %d\n", #cond, __FUNCTION__, __FILE__, __LINE__); \
            __builtin_trap();                                                                                  \
        }                                                                                                      \
    } while (0);
