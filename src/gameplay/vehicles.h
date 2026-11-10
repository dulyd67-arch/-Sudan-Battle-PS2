#ifndef GAMEPLAY_VEHICLES_H
#define GAMEPLAY_VEHICLES_H
#include "../common.h"
typedef enum { VEH_PICKUP_HILUX = 0, VEH_DESERT_BUGGY, VEH_MOTORCYCLE, VEH_MILITARY_TRUCK } VehicleClass;
typedef struct { int id; VehicleClass vclass; const char* name; Vec3 pos; float yaw, speed; int health, occupied; } Vehicle;
void vehicles_init(void);
void vehicles_update(Actor* driver, float throttle, float steering);
void vehicles_try_enter_or_exit(Actor* player);
const Vehicle* vehicles_get_all(int* count);
#endif
