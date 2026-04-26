#include "ak/ak.h"

void str_destroy(mem_allocator_t* alloc, str_t s)
{
    mem_free(alloc, s.ptr);
}

strview_t* str_split_lines(mem_allocator_t* alloc, str_t s, size_t* out_count)
{
    // Count lines
    size_t count = 1; // Start with one
    for (size_t i = 0; i < s.len; i++) {
        if (s.ptr[i] == '\n') {
            count++;
        }
    }
    if (s.len > 0 && s.ptr[s.len - 1] != '\n') {
        count++; // last line without newline
    }

    strview_t* lines = mem_alloc(alloc, sizeof(strview_t) * count);
    mem_zero(lines, sizeof(strview_t) * count);

    size_t start = 0;
    size_t line_index = 0;
    for (size_t i = 0; i < s.len; i++) {
        if (s.ptr[i] == '\n') {
            lines[line_index++] = (strview_t) {
                .ptr = &s.ptr[start],
                .len = i - start
            };
            start = i + 1;
        }
    }

    // Last line
    if (start < s.len) {
        lines[line_index++] = (strview_t) {
            .ptr = &s.ptr[start],
            .len = s.len - start
        };
    }

    *out_count = count;
    return lines;
}
