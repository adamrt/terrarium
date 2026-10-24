// Copyright (c) 2026 Adam Patterson
// SPDX-License-Identifier: ISC

#include "ak/ak.h"

#include <stdio.h>
#include <sys/stat.h>

typedef struct {
    usize size;
    usize modified;
} io_stat_t;

static io_stat_t io_file_stat(mem_allocator_t* alloc, strview_t path)
{
    struct stat st = { 0 };
    char* copy = strview_to_cstr(alloc, path);
    int rt = stat(copy, &st);
    ASSERT(rt == 0);
    ASSERT(st.st_size > 0);

    mem_free(alloc, copy);

    return (io_stat_t) {
        .size = (usize)st.st_size,
        .modified = (usize)st.st_mtime,
    };
}

str_t io_file_readall(mem_allocator_t* alloc, strview_t path)
{
    io_stat_t stat = io_file_stat(alloc, path);
    str_t str = { 0 };
    str.ptr = mem_alloc(alloc, stat.size);
    str.alloc = alloc;
    str.len = stat.size;
    str.cap = stat.size;

    char* copy = strview_to_cstr(alloc, path);
    FILE* f = fopen(copy, "r");
    ASSERT(f);
    mem_free(alloc, copy);

    usize n = fread(str.ptr, sizeof(char), str.len, f);
    ASSERT(n == str.len);

    fclose(f);

    if (str.ptr[str.len - 1] == '\0') {
        str.len--;
    }

    return str;
}
