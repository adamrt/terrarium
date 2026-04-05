#include "ak/assert.h"
#include "ak/mem.h"
#include "ak/types.h"

#include <stdlib.h>

static const u32 MEM_DEBUG_MAGIC = 0xA110CAA1;

typedef struct header {
    u32 magic;
    i32 line;
    size_t size;
    const char* file;
    struct header* next;
} header_t;

typedef struct {
    header_t* head;

    size_t allocation_count_total;
    size_t allocation_count_current;
    size_t allocation_size_total;
    size_t allocation_size_current;
} state_t;

void* debug_alloc(void* ctx, size_t size, const char* file, i32 line)
{
    ASSERT(ctx);
    ASSERT(size > 0);
    ASSERT(size + sizeof(header_t) < SIZE_MAX);

    state_t* state = ctx;

    // Header and user capacity
    header_t* header = calloc(1, size + sizeof(header_t));
    ASSERT(header);

    header->magic = MEM_DEBUG_MAGIC;
    header->file = file;
    header->line = line;
    header->size = size;
    header->next = state->head;

    state->allocation_count_total++;
    state->allocation_count_current++;
    state->allocation_size_total += size;
    state->allocation_size_current += size;

    state->head = header;

    void* user_ptr = header + 1;
    return user_ptr;
}

void debug_free(void* ctx, void* ptr, const char* file, i32 line)
{
    ASSERT(ctx);
    ASSERT(ptr);

    state_t* state = ctx;

    header_t* header = (header_t*)ptr - 1;
    ASSERT(header->magic == MEM_DEBUG_MAGIC, "Bad free '%s' @ line %d\n", file, line);

    state->allocation_count_current--;
    state->allocation_size_current -= header->size;

    bool found = false;
    header_t** current = &(state->head);
    while (*current) {
        if (*current == header) {
            *current = header->next;
            found = true;
            break;
        }
        current = &((*current)->next);
    }
    ASSERT(found);

    free(header);
}

void debug_shutdown(void* ctx)
{
    state_t* state = ctx;
    if (state->allocation_count_current || state->allocation_size_current) {
        printf("There are still %zu allocations worth %zu bytes\n", state->allocation_count_current, state->allocation_size_current);
    }
}

mem_allocator_t* mem_debug_create(void)
{

    state_t* state = calloc(1, sizeof(*state));
    ASSERT(state);

    mem_allocator_t* alloc = calloc(1, sizeof(*alloc));
    ASSERT(alloc);

    alloc->func_alloc = debug_alloc;
    alloc->func_free = debug_free;
    alloc->func_shutdown = debug_shutdown;
    alloc->ctx = state;

    return alloc;
}
