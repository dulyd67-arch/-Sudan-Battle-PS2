#ifndef ENGINE_WORLD_STREAM_H
#define ENGINE_WORLD_STREAM_H
#include "../common.h"
typedef struct { int id; const char* name; Vec3 center; int is_loaded, building_count; AABB buildings[8]; } WorldSector;
void world_stream_init(void);
void world_stream_update(Vec3 pos);
const WorldSector* world_stream_get_sectors(int* count);
const char* world_stream_get_current_zone_name(Vec3 pos);
#endif
