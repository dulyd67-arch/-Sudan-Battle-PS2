#include "camera.h"
#include "../physics/collision.h"
void camera_init(ThirdPersonCamera* cam) {
    cam->position = (Vec3){0.0f, 8.0f, -12.0f};
    cam->target = (Vec3){0.0f, 2.0f, 0.0f};
    cam->yaw = 0.0f; cam->pitch = 0.25f; cam->distance = 9.0f;
}
void camera_update(ThirdPersonCamera* cam, const Actor* target, float rx, float ry, int aiming) {
    cam->yaw += rx * 0.045f;
    cam->pitch += ry * 0.035f;
    if (cam->pitch < -0.6f) cam->pitch = -0.6f;
    if (cam->pitch > 1.1f) cam->pitch = 1.1f;
    float dist = aiming ? 4.2f : 9.0f;
    cam->distance += (dist - cam->distance) * 0.2f;
    float h = (target->stance == 2) ? 0.8f : ((target->stance == 1) ? 1.4f : 2.2f);
    cam->target = (Vec3){target->pos.x, target->pos.y + h, target->pos.z};
    float hd = cam->distance * cosf(cam->pitch), vd = cam->distance * sinf(cam->pitch);
    Vec3 ideal = {cam->target.x - sinf(cam->yaw) * hd, cam->target.y + vd, cam->target.z - cosf(cam->yaw) * hd};
    float gh = physics_sample_terrain_height(ideal.x, ideal.z);
    if (ideal.y < gh + 0.8f) ideal.y = gh + 0.8f;
    cam->position = ideal;
}
int camera_frustum_contains_point(const ThirdPersonCamera* cam, Vec3 pt, float max_dist) {
    if (vec3_dist_sq(cam->position, pt) > max_dist * max_dist) return 0;
    Vec3 to = vec3_sub(pt, cam->position);
    Vec3 fwd = {sinf(cam->yaw) * cosf(cam->pitch), -sinf(cam->pitch), cosf(cam->yaw) * cosf(cam->pitch)};
    return ((to.x * fwd.x + to.y * fwd.y + to.z * fwd.z) > -5.0f) ? 1 : 0;
}
