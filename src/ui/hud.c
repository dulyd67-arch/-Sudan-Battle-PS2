#include "hud.h"
#include "../gameplay/combat.h"
#include "../engine/world_stream.h"
#include "../engine/memory.h"
#include <stdio.h>
void hud_log_telemetry(const BattleRoyaleMatch* m) {
    if (m->frame_counter % 180 != 0) return;
    const WeaponSpec* w = combat_get_weapon_spec(m->player.current_weapon);
    printf("[SUDAN BATTLE PS2] State:%d | Zone:%s | HP:%d | Wpn:%s | RAM:%u KB\n",
           m->state, world_stream_get_current_zone_name(m->player.pos), m->player.health, w->name, mem_get_used_bytes() / 1024);
}
