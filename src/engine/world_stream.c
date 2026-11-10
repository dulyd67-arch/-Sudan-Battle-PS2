#include "world_stream.h"
static WorldSector g_sectors[MAX_SECTORS] = {
    {0, "Omdurman Old Souq", {-300.0f, 0.0f, 300.0f}, 0, 4, {}},
    {1, "Nile Bridge", {0.0f, 0.0f, 300.0f}, 0, 2, {}},
    {2, "Bahri Industrial", {300.0f, 0.0f, 300.0f}, 0, 5, {}},
    {3, "Western Desert", {-300.0f, 0.0f, 0.0f}, 0, 1, {}},
    {4, "Khartoum Central", {0.0f, 0.0f, 0.0f}, 0, 6, {}},
    {5, "Blue Nile Docks", {300.0f, 0.0f, 0.0f}, 0, 3, {}},
    {6, "Jebel Awliya", {-300.0f, 0.0f, -300.0f}, 0, 2, {}},
    {7, "Gezira Rural", {0.0f, 0.0f, -300.0f}, 0, 4, {}},
    {8, "Military Bunker", {300.0f, 0.0f, -300.0f}, 0, 5, {}}
};
void world_stream_init(void) {
    for (int i = 0; i < MAX_SECTORS; i++) {
        g_sectors[i].is_loaded = 0;
        for (int b = 0; b < g_sectors[i].building_count; b++) {
            float ox = (float)((b % 3) - 1) * 45.0f, oz = (float)((b / 3) - 1) * 45.0f;
            float cx = g_sectors[i].center.x + ox, cz = g_sectors[i].center.z + oz;
            g_sectors[i].buildings[b].min = (Vec3){cx - 12.0f, 0.0f, cz - 12.0f};
            g_sectors[i].buildings[b].max = (Vec3){cx + 12.0f, 14.0f + (float)(b * 3), cz + 12.0f};
        }
    }
}
void world_stream_update(Vec3 pos) {
    for (int i = 0; i < MAX_SECTORS; i++) {
        float d2 = vec3_dist_sq(pos, g_sectors[i].center);
        if (!g_sectors[i].is_loaded && d2 <= 380.0f * 380.0f) g_sectors[i].is_loaded = 1;
        else if (g_sectors[i].is_loaded && d2 > 440.0f * 440.0f) g_sectors[i].is_loaded = 0;
    }
}
const WorldSector* world_stream_get_sectors(int* count) { if (count) *count = MAX_SECTORS; return g_sectors; }
const char* world_stream_get_current_zone_name(Vec3 pos) {
    int best = 4; float best_d2 = 1e12f;
    for (int i = 0; i < MAX_SECTORS; i++) {
        float d2 = vec3_dist_sq(pos, g_sectors[i].center);
        if (d2 < best_d2) { best_d2 = d2; best = i; }
    }
    return g_sectors[best].name;
}
