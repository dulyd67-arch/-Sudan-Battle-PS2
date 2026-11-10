#include "match.h"
#include "combat.h"
#include "vehicles.h"
#include "../ai/bot_ai.h"
#include "../physics/collision.h"
#include "../engine/world_stream.h"
static void spawn_loot(BattleRoyaleMatch* m) {
    for (int i = 0; i < MAX_LOOT_ITEMS; i++) {
        float angle = (float)i * 0.55f, radius = 25.0f + (float)((i * 17) % 280);
        m->loot[i].pos = (Vec3){cosf(angle) * radius, 0.0f, sinf(angle) * radius};
        m->loot[i].pos.y = physics_sample_terrain_height(m->loot[i].pos.x, m->loot[i].pos.z);
        m->loot[i].weapon_type = (WeaponType)(1 + (i % 5));
        m->loot[i].armor_bonus = (i % 3 == 0) ? 50 : 25;
        m->loot[i].medkit_bonus = (i % 4 == 0) ? 1 : 0;
        m->loot[i].active = 1;
    }
}
void match_init(BattleRoyaleMatch* m) {
    m->state = STATE_MAIN_MENU; m->frame_counter = 0;
    m->safe_zone_center = (Vec3){0.0f, 0.0f, 0.0f}; m->safe_zone_radius = 480.0f;
    m->player.pos = (Vec3){-220.0f, 180.0f, -120.0f};
    m->player.yaw = 0.0f; m->player.pitch = 0.0f;
    m->player.health = 100; m->player.armor = 50; m->player.alive = 1;
    m->player.stance = 0; m->player.in_vehicle_id = -1; m->player.kills = 0;
    m->player.medkits = 2; m->player.grenades = 2;
    combat_equip_weapon(&m->player, WPN_PISTOL_9MM);
    ai_init_bots(m->bots, MAX_BOTS);
    spawn_loot(m);
    vehicles_init();
    world_stream_init();
    m->alive_count = MAX_BOTS + 1;
}
void match_update(BattleRoyaleMatch* m, const PadState* pad, ThirdPersonCamera* cam) {
    m->frame_counter++;
    if (m->player.fire_timer > 0) m->player.fire_timer--;
    if (m->state == STATE_MAIN_MENU) {
        if (pad->buttons_pressed & (PAD_START | PAD_CROSS)) m->state = STATE_DEPLOYMENT_AIRCRAFT;
        return;
    }
    if (m->state == STATE_DEPLOYMENT_AIRCRAFT) {
        m->player.pos.x += 1.4f; m->player.pos.z += 0.8f;
        camera_update(cam, &m->player, pad->right_stick_x, pad->right_stick_y, 0);
        if ((pad->buttons_pressed & PAD_CROSS) || m->player.pos.x > 220.0f) m->state = STATE_PARACHUTE;
        return;
    }
    if (m->state == STATE_PARACHUTE) {
        m->player.pos.x += pad->left_stick_x * 1.1f;
        m->player.pos.z -= pad->left_stick_y * 1.1f;
        m->player.pos.y -= 0.85f;
        camera_update(cam, &m->player, pad->right_stick_x, pad->right_stick_y, 0);
        world_stream_update(m->player.pos);
        float ground = physics_sample_terrain_height(m->player.pos.x, m->player.pos.z);
        if (m->player.pos.y <= ground + 0.5f) { m->player.pos.y = ground; m->state = STATE_ACTIVE_COMBAT; }
        return;
    }
    if (m->state == STATE_VICTORY || m->state == STATE_ELIMINATED) {
        if (pad->buttons_pressed & PAD_START) match_init(m);
        return;
    }
    int aiming = (pad->buttons_held & PAD_L1) ? 1 : 0;
    camera_update(cam, &m->player, pad->right_stick_x, pad->right_stick_y, aiming);
    m->player.yaw = cam->yaw;
    if (pad->buttons_pressed & PAD_TRIANGLE) vehicles_try_enter_or_exit(&m->player);
    if (m->player.in_vehicle_id >= 0) {
        vehicles_update(&m->player, -pad->left_stick_y, pad->left_stick_x);
    } else {
        float speed = (pad->buttons_held & PAD_L3) ? 0.95f : 0.55f;
        if (m->player.stance == 1) speed *= 0.65f;
        if (m->player.stance == 2) speed *= 0.35f;
        float fwd = -pad->left_stick_y * speed, str = pad->left_stick_x * speed;
        m->player.pos.x += sinf(m->player.yaw) * fwd + cosf(m->player.yaw) * str;
        m->player.pos.z += cosf(m->player.yaw) * fwd - sinf(m->player.yaw) * str;
        physics_resolve_actor_world(&m->player);
        if (pad->buttons_pressed & PAD_CIRCLE) m->player.stance = (m->player.stance + 1) % 3;
        if (pad->buttons_pressed & PAD_SQUARE) combat_reload(&m->player);
        if (pad->buttons_pressed & PAD_UP) combat_use_medkit(&m->player);
        if (pad->buttons_held & PAD_R1) {
            Vec3 dir = {sinf(cam->yaw) * cosf(cam->pitch), -sinf(cam->pitch), cosf(cam->yaw) * cosf(cam->pitch)};
            combat_try_fire(&m->player, dir, m->bots, MAX_BOTS);
        }
        for (int i = 0; i < MAX_LOOT_ITEMS; i++) {
            if (m->loot[i].active && vec3_dist_sq(m->player.pos, m->loot[i].pos) < 9.0f) {
                combat_equip_weapon(&m->player, m->loot[i].weapon_type);
                m->player.armor += m->loot[i].armor_bonus;
                if (m->player.armor > 100) m->player.armor = 100;
                m->player.medkits += m->loot[i].medkit_bonus;
                m->loot[i].active = 0;
            }
        }
    }
    world_stream_update(m->player.pos);
    if (m->safe_zone_radius > 35.0f) m->safe_zone_radius -= 0.045f;
    if (m->frame_counter % 60 == 0) {
        if (vec3_dist_sq(m->player.pos, m->safe_zone_center) > m->safe_zone_radius * m->safe_zone_radius) {
            m->player.health -= 4;
            if (m->player.health <= 0) { m->player.health = 0; m->player.alive = 0; m->state = STATE_ELIMINATED; }
        }
    }
    ai_update_bots_timesliced(m->bots, MAX_BOTS, &m->player, m->safe_zone_center, m->safe_zone_radius, m->frame_counter);
    int bots_alive = 0;
    for (int i = 0; i < MAX_BOTS; i++) if (m->bots[i].alive) bots_alive++;
    m->alive_count = bots_alive + (m->player.alive ? 1 : 0);
    if (!m->player.alive) m->state = STATE_ELIMINATED;
    else if (bots_alive == 0) m->state = STATE_VICTORY;
}
