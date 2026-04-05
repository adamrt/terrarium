#pragma once

#include "ak/types.h"

typedef struct {
    void* (*func_alloc)(void* ctx, size_t size, const char* file, i32 line);
    void (*func_free)(void* ctx, void* ptr, const char* file, i32 line);
    void (*func_shutdown)(void* ctx);

    void* ctx;
} mem_allocator_t;

mem_allocator_t* mem_heap_create(void);

#define mem_alloc(a, size) ((a)->func_alloc((a)->ctx, size, __FILE__, __LINE__))
#define mem_free(a, ptr)   ((a)->func_free((a)->ctx, ptr, __FILE__, __LINE__))
#define mem_shutdown(a)                   \
    do {                                  \
        if ((a)->func_shutdown) {         \
            (a)->func_shutdown((a)->ctx); \
        }                                 \
    } while (0);
