#include "collision.h"
#include "../engine/world_stream.h"
float physics_sample_terrain_height(float x, float z) {
    if (x > 35.0f && x < 85.0f) return -2.5f;
    if (x < -180.0f && z < -180.0f) return 8.0f + sinf(x * 0.02f) * 4.0f;
    return sinf(x * 0.01f) * cosf(z * 0.01f) * 1.2f;
}
void physics_resolve_actor_world(Actor* actor) {
    float ground = physics_sample_terrain_height(actor->pos.x, actor->pos.z);
    if (actor->pos.y < ground) { actor->pos.y = ground; actor->vel.y = 0.0f; }
    int count = 0;
    const WorldSector* sectors = world_stream_get_sectors(&count);
    for (int s = 0; s < count; s++) {
        if (!sectors[s].is_loaded) continue;
        for (int b = 0; b < sectors[s].building_count; b++) {
            AABB box = sectors[s].buildings[b];
            if (actor->pos.x > box.min.x - 1.0f && actor->pos.x < box.max.x + 1.0f &&
                actor->pos.z > box.min.z - 1.0f && actor->pos.z < box.max.z + 1.0f &&
                actor->pos.y < box.max.y) {
                if ((actor->pos.x - box.min.x) < (box.max.x - actor->pos.x)) actor->pos.x = box.min.x - 1.05f;
                else actor->pos.x = box.max.x + 1.05f;
            }
        }
    }
}
int physics_raycast_hit(Vec3 origin, Vec3 dir, float max_dist, Vec3 target_pos, float target_radius) {
    Vec3 to = vec3_sub(target_pos, origin);
    float proj = to.x * dir.x + to.y * dir.y + to.z * dir.z;
    if (proj < 0.0f || proj > max_dist) return 0;
    Vec3 closest = vec3_add(origin, vec3_scale(dir, proj));
    return (vec3_dist_sq(closest, target_pos) <= target_radius * target_radius) ? 1 : 0;
}
