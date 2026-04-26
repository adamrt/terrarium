// Copyright 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"

#include <stdlib.h>

void* heap_alloc(void* ctx, size_t size, const char* file, i32 line)
{
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(line);

    void* ptr = malloc(size);
    ASSERT(ptr);

    return ptr;
}

void* heap_realloc(void* ctx, void* ptr, size_t size, const char* file, i32 line)
{
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(line);

    ASSERT(ptr);

    ptr = realloc(ptr, size);
    ASSERT(ptr);

    return ptr;
}

void heap_free(void* ctx, void* ptr)
{
    UNUSED(ctx);

    ASSERT(ptr);

    free(ptr);
}

void heap_shutdown(struct mem_allocator* self)
{
    ASSERT(self);

    free(self);
}

mem_allocator_t* mem_heap_create(void)
{
    mem_allocator_t* alloc = calloc(1, sizeof(*alloc));
    ASSERT(alloc);

    alloc->func_alloc = heap_alloc;
    alloc->func_realloc = heap_realloc;
    alloc->func_free = heap_free;
    alloc->func_shutdown = heap_shutdown;
    alloc->ctx = NULL;

    return alloc;
}
