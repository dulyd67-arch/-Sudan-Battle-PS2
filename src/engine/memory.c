#include "memory.h"
#include <string.h>
static u8 g_engine_pool[ENGINE_POOL_SIZE] __attribute__((aligned(64)));
static u32 g_pool_offset = 0;
void mem_init(void) { memset(g_engine_pool, 0, sizeof(g_engine_pool)); g_pool_offset = 0; }
void* mem_alloc(u32 size) {
    u32 sz = (size + 15) & ~15;
    if (g_pool_offset + sz > ENGINE_POOL_SIZE) return 0;
    void* p = &g_engine_pool[g_pool_offset];
    g_pool_offset += sz;
    return p;
}
u32 mem_get_used_bytes(void) { return g_pool_offset; }
