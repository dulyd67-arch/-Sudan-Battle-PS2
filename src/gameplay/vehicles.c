#include "vehicles.h"
#include "../physics/collision.h"
static Vehicle g_vehicles[MAX_VEHICLES] = {
    {0, VEH_PICKUP_HILUX, "Desert Pickup 4x4", {-40.0f, 0.0f, 40.0f}, 0.0f, 0.0f, 400, 0},
    {1, VEH_DESERT_BUGGY, "Khartoum Buggy", {95.0f, 0.0f, -20.0f}, 1.2f, 0.0f, 280, 0},
    {2, VEH_MOTORCYCLE, "Scout Bike", {-150.0f, 0.0f, 180.0f}, 0.5f, 0.0f, 160, 0},
    {3, VEH_MILITARY_TRUCK, "Armored Truck", {210.0f, 0.0f, 210.0f}, 3.1f, 0.0f, 650, 0},
    {4, VEH_PICKUP_HILUX, "Nile Pickup", {15.0f, 0.0f, 275.0f}, 0.0f, 0.0f, 400, 0},
    {5, VEH_MOTORCYCLE, "Nomad Bike", {-240.0f, 0.0f, -220.0f}, 2.0f, 0.0f, 160, 0}
};
void vehicles_init(void) {
    for (int i = 0; i < MAX_VEHICLES; i++) {
        g_vehicles[i].pos.y = physics_sample_terrain_height(g_vehicles[i].pos.x, g_vehicles[i].pos.z);
        g_vehicles[i].speed = 0.0f; g_vehicles[i].occupied = 0;
    }
}
void vehicles_try_enter_or_exit(Actor* player) {
    if (player->in_vehicle_id >= 0) {
        int vid = player->in_vehicle_id;
        g_vehicles[vid].occupied = 0; g_vehicles[vid].speed = 0.0f;
        player->in_vehicle_id = -1; player->pos.x += 2.8f;
        return;
    }
    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (g_vehicles[i].health > 0 && vec3_dist_sq(player->pos, g_vehicles[i].pos) < 16.0f) {
            player->in_vehicle_id = i; g_vehicles[i].occupied = 1;
            player->pos = g_vehicles[i].pos;
            return;
        }
    }
}
void vehicles_update(Actor* driver, float throttle, float steering) {
    if (driver->in_vehicle_id < 0 || driver->in_vehicle_id >= MAX_VEHICLES) return;
    Vehicle* v = &g_vehicles[driver->in_vehicle_id];
    v->speed += throttle * 0.08f; v->speed *= 0.96f;
    if (v->speed > 1.8f) v->speed = 1.8f;
    if (v->speed < -0.7f) v->speed = -0.7f;
    v->yaw += steering * 0.045f * (v->speed >= 0.0f ? 1.0f : -1.0f);
    v->pos.x += sinf(v->yaw) * v->speed;
    v->pos.z += cosf(v->yaw) * v->speed;
    v->pos.y = physics_sample_terrain_height(v->pos.x, v->pos.z);
    driver->pos = v->pos; driver->yaw = v->yaw;
}
const Vehicle* vehicles_get_all(int* count) { if (count) *count = MAX_VEHICLES; return g_vehicles; }
