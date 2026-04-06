#pragma once

#include "ak/mem.h"
#include "ak/types.h"

typedef struct ws_server ws_server_t;

// Lifecycle
ws_server_t* ws_server_create(mem_allocator_t* alloc, i32 width, i32 height);
void ws_server_destroy(mem_allocator_t* alloc, ws_server_t* server);
void ws_server_render(ws_server_t* server);
