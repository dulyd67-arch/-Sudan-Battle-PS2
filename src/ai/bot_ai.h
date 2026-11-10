#ifndef AI_BOT_AI_H
#define AI_BOT_AI_H
#include "../common.h"
void ai_init_bots(Actor* bots, int count);
void ai_update_bots_timesliced(Actor* bots, int count, Actor* player, Vec3 safe_center, float safe_radius, int frame_counter);
#endif
