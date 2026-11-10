#ifndef GAMEPLAY_MATCH_H
#define GAMEPLAY_MATCH_H
#include "../common.h"
#include "../engine/input.h"
#include "../engine/camera.h"
typedef struct { Vec3 pos; WeaponType weapon_type; int armor_bonus, medkit_bonus, active; } LootCrate;
typedef struct {
    MatchState state;
    Actor player;
    Actor bots[MAX_BOTS];
    LootCrate loot[MAX_LOOT_ITEMS];
    Vec3 safe_zone_center;
    float safe_zone_radius;
    int alive_count, frame_counter;
} BattleRoyaleMatch;
void match_init(BattleRoyaleMatch* m);
void match_update(BattleRoyaleMatch* m, const PadState* pad, ThirdPersonCamera* cam);
#endif
