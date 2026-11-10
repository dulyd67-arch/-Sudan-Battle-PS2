#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H
#include "../common.h"
typedef struct { Vec3 position, target; float yaw, pitch, distance; } ThirdPersonCamera;
void camera_init(ThirdPersonCamera* cam);
void camera_update(ThirdPersonCamera* cam, const Actor* target, float rx, float ry, int aiming);
int camera_frustum_contains_point(const ThirdPersonCamera* cam, Vec3 pt, float max_dist);
#endif
