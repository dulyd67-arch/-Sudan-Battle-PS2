#include "bot_ai.h"
#include "../gameplay/combat.h"
#include "../physics/collision.h"
void ai_init_bots(Actor* bots, int count) {
    for (int i = 0; i < count; i++) {
        float angle = (float)i * (6.28318f / (float)count), dist = 110.0f + (float)((i % 4) * 55);
        bots[i].pos = (Vec3){cosf(angle) * dist, 0.0f, sinf(angle) * dist};
        bots[i].pos.y = physics_sample_terrain_height(bots[i].pos.x, bots[i].pos.z);
        bots[i].yaw = angle + 3.14159f; bots[i].pitch = 0.0f;
        bots[i].health = 100; bots[i].armor = (i % 2 == 0) ? 50 : 25;
        bots[i].alive = 1; bots[i].stance = 0; bots[i].in_vehicle_id = -1;
        bots[i].kills = 0; bots[i].medkits = 1;
        combat_equip_weapon(&bots[i], (WeaponType)(1 + (i % 5)));
    }
}
void ai_update_bots_timesliced(Actor* bots, int count, Actor* player, Vec3 safe_center, float safe_radius, int fc) {
    for (int i = 0; i < count; i++) {
        if (!bots[i].alive) continue;
        if (bots[i].fire_timer > 0) bots[i].fire_timer--;
        if ((i + fc) % 3 != 0) continue;
        Actor* b = &bots[i];
        if (b->health < 35 && b->medkits > 0) { combat_use_medkit(b); continue; }
        if (vec3_dist_sq(b->pos, safe_center) > (safe_radius * 0.85f) * (safe_radius * 0.85f)) {
            Vec3 tc = vec3_sub(safe_center, b->pos);
            float len = sqrtf(tc.x * tc.x + tc.z * tc.z);
            if (len > 0.001f) {
                b->pos.x += (tc.x / len) * 0.95f;
                b->pos.z += (tc.z / len) * 0.95f;
                b->yaw = atan2f(tc.x, tc.z);
            }
            physics_resolve_actor_world(b);
            continue;
        }
        Actor* target = 0; float best_d2 = 95.0f * 95.0f;
        if (player->alive && vec3_dist_sq(b->pos, player->pos) < best_d2) {
            best_d2 = vec3_dist_sq(b->pos, player->pos); target = player;
        }
        for (int j = 0; j < count; j++) {
            if (i == j || !bots[j].alive) continue;
            float bd2 = vec3_dist_sq(b->pos, bots[j].pos);
            if (bd2 < best_d2) { best_d2 = bd2; target = &bots[j]; }
        }
        if (target) {
            Vec3 diff = vec3_sub(target->pos, b->pos);
            float d = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            if (d > 0.1f) {
                Vec3 dir = vec3_scale(diff, 1.0f / d);
                b->yaw = atan2f(dir.x, dir.z);
                b->pos.x += cosf(b->yaw) * 0.35f;
                b->pos.z -= sinf(b->yaw) * 0.35f;
                combat_try_fire(b, dir, target, 1);
            }
        } else {
            b->pos.x += sinf(b->yaw) * 0.45f;
            b->pos.z += cosf(b->yaw) * 0.45f;
        }
        physics_resolve_actor_world(b);
    }
}
