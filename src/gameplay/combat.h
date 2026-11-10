#ifndef GAMEPLAY_COMBAT_H
#define GAMEPLAY_COMBAT_H
#include "../common.h"
const WeaponSpec* combat_get_weapon_spec(WeaponType type);
void combat_equip_weapon(Actor* actor, WeaponType type);
void combat_reload(Actor* actor);
int combat_try_fire(Actor* shooter, Vec3 aim_dir, Actor* targets, int target_count);
void combat_use_medkit(Actor* actor);
#endif
