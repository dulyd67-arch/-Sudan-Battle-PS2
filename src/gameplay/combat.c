#include "combat.h"
#include "../physics/collision.h"
static const WeaponSpec g_weapons[WPN_COUNT] = {
    {WPN_NONE, "Unarmed", 5, 0, 20, 3.5f, 0.00f},
    {WPN_PISTOL_9MM, "Karari 9mm", 18, 12, 12, 85.0f, 0.02f},
    {WPN_SMG_SCORPION, "Desert SMG", 14, 30, 5, 65.0f, 0.015f},
    {WPN_AR_KALASH, "Nile Kalash AR", 28, 30, 7, 160.0f, 0.03f},
    {WPN_SHOTGUN_TACTICAL, "Breacher 12G", 62, 6, 28, 32.0f, 0.06f},
    {WPN_SNIPER_FALCON, "Sabaho Sniper", 85, 5, 42, 320.0f, 0.08f}
};
const WeaponSpec* combat_get_weapon_spec(WeaponType type) {
    if (type < 0 || type >= WPN_COUNT) return &g_weapons[0];
    return &g_weapons[type];
}
void combat_equip_weapon(Actor* actor, WeaponType type) {
    const WeaponSpec* spec = combat_get_weapon_spec(type);
    actor->current_weapon = type;
    actor->ammo_in_clip = spec->clip_size;
    actor->reserve_ammo = spec->clip_size * 3;
}
void combat_reload(Actor* actor) {
    const WeaponSpec* spec = combat_get_weapon_spec(actor->current_weapon);
    if (spec->clip_size == 0 || actor->reserve_ammo <= 0) return;
    int needed = spec->clip_size - actor->ammo_in_clip;
    if (needed <= 0) return;
    int transfer = (actor->reserve_ammo >= needed) ? needed : actor->reserve_ammo;
    actor->ammo_in_clip += transfer;
    actor->reserve_ammo -= transfer;
    actor->fire_timer = 25;
}
int combat_try_fire(Actor* shooter, Vec3 aim_dir, Actor* targets, int target_count) {
    if (shooter->fire_timer > 0) return 0;
    const WeaponSpec* spec = combat_get_weapon_spec(shooter->current_weapon);
    if (spec->clip_size > 0) {
        if (shooter->ammo_in_clip <= 0) { combat_reload(shooter); return 0; }
        shooter->ammo_in_clip--;
    }
    shooter->fire_timer = spec->fire_cooldown_frames;
    shooter->pitch -= spec->recoil_pitch;
    Vec3 muzzle = {shooter->pos.x, shooter->pos.y + 1.6f, shooter->pos.z};
    int hits = 0;
    for (int i = 0; i < target_count; i++) {
        if (!targets[i].alive) continue;
        Vec3 tc = {targets[i].pos.x, targets[i].pos.y + 1.2f, targets[i].pos.z};
        if (physics_raycast_hit(muzzle, aim_dir, spec->range, tc, 1.6f)) {
            int dmg = spec->damage;
            if (targets[i].armor > 0) {
                int abs = (dmg / 2 > targets[i].armor) ? targets[i].armor : dmg / 2;
                targets[i].armor -= abs;
                dmg -= abs;
            }
            targets[i].health -= dmg;
            if (targets[i].health <= 0) { targets[i].health = 0; targets[i].alive = 0; shooter->kills++; }
            hits++;
        }
    }
    return hits;
}
void combat_use_medkit(Actor* actor) {
    if (actor->medkits > 0 && actor->health < 100) {
        actor->medkits--;
        actor->health += 50;
        if (actor->health > 100) actor->health = 100;
    }
}
