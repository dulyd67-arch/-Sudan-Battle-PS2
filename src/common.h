#ifndef SUDAN_BATTLE_COMMON_H
#define SUDAN_BATTLE_COMMON_H
#include <tamtypes.h>
#include <math.h>
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 448
#define MAX_BOTS       15
#define MAX_LOOT_ITEMS 48
#define MAX_VEHICLES   6
#define MAX_SECTORS    9
typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 min; Vec3 max; } AABB;
typedef enum { STATE_MAIN_MENU = 0, STATE_DEPLOYMENT_AIRCRAFT, STATE_PARACHUTE, STATE_ACTIVE_COMBAT, STATE_VICTORY, STATE_ELIMINATED } MatchState;
typedef enum { WPN_NONE = 0, WPN_PISTOL_9MM, WPN_SMG_SCORPION, WPN_AR_KALASH, WPN_SHOTGUN_TACTICAL, WPN_SNIPER_FALCON, WPN_COUNT } WeaponType;
typedef struct { WeaponType type; const char* name; int damage; int clip_size; int fire_cooldown_frames; float range; float recoil_pitch; } WeaponSpec;
typedef struct { Vec3 pos; Vec3 vel; float yaw; float pitch; int health; int armor; int alive; int stance; int in_vehicle_id; WeaponType current_weapon; int ammo_in_clip; int reserve_ammo; int fire_timer; int kills; int medkits; int grenades; } Actor;
static inline Vec3 vec3_add(Vec3 a, Vec3 b) { Vec3 r = {a.x + b.x, a.y + b.y, a.z + b.z}; return r; }
static inline Vec3 vec3_sub(Vec3 a, Vec3 b) { Vec3 r = {a.x - b.x, a.y - b.y, a.z - b.z}; return r; }
static inline Vec3 vec3_scale(Vec3 v, float s) { Vec3 r = {v.x * s, v.y * s, v.z * s}; return r; }
static inline float vec3_dist_sq(Vec3 a, Vec3 b) { float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z; return dx*dx + dy*dy + dz*dz; }
#endif
