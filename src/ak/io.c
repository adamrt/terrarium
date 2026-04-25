#include "ak/ak.h"

#include <sys/stat.h>

typedef struct {
    size_t size;
    size_t modified;
} io_stat_t;

io_stat_t io_file_stat(mem_allocator_t* alloc, strview_t path)
{
    struct stat st = { 0 };
    char* copy = strview_to_cstr(alloc, path);
    int rt = stat(copy, &st);
    ASSERT(rt == 0);
    ASSERT(st.st_size > 0);

    mem_free(alloc, copy);

    return (io_stat_t) {
        .size = (size_t)st.st_size,
        .modified = (size_t)st.st_mtime,
    };
}

str_t io_file_readall(mem_allocator_t* alloc, strview_t path)
{
    io_stat_t st = io_file_stat(alloc, path);
    str_t str = { 0 };
    str.ptr = mem_alloc(alloc, st.size);
    str.len = st.size;
    str.cap = st.size;

    char* copy = strview_to_cstr(alloc, path);
    FILE* f = fopen(copy, "r");
    ASSERT(f);
    mem_free(alloc, copy);

    size_t n = fread(str.ptr, sizeof(char), str.len, f);
    ASSERT(n > 0);

    if (str.ptr[str.len - 1] == '\0') {
        str.len--;
    }

    return str;
}
