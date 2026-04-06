#pragma once

#include "ak/types.h"

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
