#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H
#include "../common.h"
float physics_sample_terrain_height(float x, float z);
void physics_resolve_actor_world(Actor* actor);
int physics_raycast_hit(Vec3 origin, Vec3 dir, float max_dist, Vec3 target_pos, float target_radius);
#endif
