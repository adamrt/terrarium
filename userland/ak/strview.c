#include "ak/ak.h"

char* strview_to_cstr(mem_allocator_t* alloc, strview_t view)
{
    char* s = mem_alloc(alloc, view.len + 1);
    memcpy(s, view.ptr, view.len);
    s[view.len] = '\0';
    return s;
}
