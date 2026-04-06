#include "ak/assert.h"
#include "ak/mem.h"
#include "ak/types.h"

#include <stdlib.h>
#include <string.h>

static const u32 MEM_MAGIC_ALLOC = 0xA110CAA1;

typedef struct header {
    alignas(max_align_t) u32 magic;
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

static void* debug_alloc(void* ctx, size_t size, const char* file, i32 line)
{
    ASSERT(ctx);
    ASSERT(size > 0);
    ASSERT(size <= SIZE_MAX - sizeof(header_t));

    state_t* state = ctx;

    header_t* header = malloc(sizeof(header_t) + size);
    ASSERT(header);

    // Memset debug values
    void* user_ptr = header + 1;
    memset(user_ptr, 0xCD, size);

    header->magic = MEM_MAGIC_ALLOC;
    header->size = size;
    header->file = file;
    header->line = line;
    header->next = state->head;

    state->head = header;

    state->allocation_count_total++;
    state->allocation_count_current++;
    state->allocation_size_total += size;
    state->allocation_size_current += size;

    return user_ptr;
}

// debug_realloc forbids NULL ptr. This will help catch unintentional memory
// issues at the expense of a little more verbose code in a handful of places.
static void* debug_realloc(void* ctx, void* ptr, size_t new_size, const char* file, int line)
{
    ASSERT(ctx);
    ASSERT(ptr);
    ASSERT(new_size > 0);
    ASSERT(new_size <= SIZE_MAX - sizeof(header_t));

    state_t* state = ctx;

    header_t* old_header = ((header_t*)ptr) - 1;
    size_t old_size = old_header->size;
    ASSERT(old_header->magic == MEM_MAGIC_ALLOC);

    header_t* header = NULL;

    bool found = false;
    header_t** current = &state->head;
    while (*current) {
        if (*current == old_header) {
            // We can't realloc before this or old_header is dangling for
            // comparison.
            header = realloc(old_header, sizeof(header_t) + new_size);
            ASSERT(header);
            *current = header;
            found = true;
            break;
        }
        current = &((*current)->next);
    }
    ASSERT(found);

    // If new size is smaller, the write 0xCD to extra old memory.
    if (new_size > old_size) {
        void* new_bytes = (char*)(header + 1) + old_size;
        memset(new_bytes, 0xCD, new_size - old_size);
    }

    header->size = new_size;
    header->file = file;
    header->line = line;

    state->allocation_size_current -= old_size;
    state->allocation_size_current += new_size;

    void* user_ptr = header + 1;
    return user_ptr;
}

static void debug_free(void* ctx, void* ptr)
{
    ASSERT(ctx);
    ASSERT(ptr);

    state_t* state = ctx;

    header_t* header = (header_t*)ptr - 1;
    ASSERT(header->magic == MEM_MAGIC_ALLOC, "Invalid pointer\n");

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

    state->allocation_count_current--;
    state->allocation_size_current -= header->size;

    free(header);
}

static void debug_shutdown(struct mem_allocator* self)
{
    state_t* state = self->ctx;

    printf("Memory usage total: %0.2fMB, allocations: %zu\n", BYTES_TO_MB(state->allocation_size_total), state->allocation_count_total);

    if (state->allocation_count_current != 0) {
        printf("Memory leak detected: %zu allocations remaining\n", state->allocation_count_current);
        header_t* current = state->head;
        while (current) {
            printf("Leaked %zu bytes allocated from %s:%d\n", current->size, current->file, current->line);
            current = current->next;
        }
    }

    if (state->allocation_size_current != 0) {
        printf("Memory leak detected: %zu bytes remaining\n", state->allocation_size_current);
    }

    free(self->ctx);
    free(self);
}

mem_allocator_t* mem_debug_create(void)
{
    state_t* state = calloc(1, sizeof(*state));
    ASSERT(state);

    mem_allocator_t* alloc = calloc(1, sizeof(*alloc));
    ASSERT(alloc);

    alloc->func_alloc = debug_alloc;
    alloc->func_realloc = debug_realloc;
    alloc->func_free = debug_free;
    alloc->func_shutdown = debug_shutdown;
    alloc->ctx = state;

    return alloc;
}
