#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H
#include <tamtypes.h>
#define ENGINE_POOL_SIZE (8 * 1024 * 1024)
void mem_init(void);
void* mem_alloc(u32 size);
u32 mem_get_used_bytes(void);
#endif
