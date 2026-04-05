#include "ak/assert.h"
#include "ak/mem.h"
#include "ak/types.h"

#include <stdlib.h>

void* heap_alloc(void* ctx, size_t size, const char* file, i32 line)
{
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(line);

    void* ptr = calloc(1, size);
    ASSERT(ptr);

    return ptr;
}

void heap_free(void* ctx, void* ptr, const char* file, i32 line)
{
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(line);

    ASSERT(ptr);

    free(ptr);
}

mem_allocator_t* mem_heap_create(void)
{
    mem_allocator_t* alloc = calloc(1, sizeof(*alloc));
    ASSERT(alloc);

    alloc->func_alloc = heap_alloc;
    alloc->func_free = heap_free;

    return alloc;
}
