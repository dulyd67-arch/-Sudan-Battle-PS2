#include <kernel.h>
#include <tamtypes.h>
#include <stdio.h>
#include "common.h"
#include "engine/memory.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/world_stream.h"
#include "render/renderer.h"
#include "gameplay/match.h"
#include "network/lan_net.h"
#include "audio/sound.h"
#include "ui/hud.h"

static BattleRoyaleMatch g_match;
static ThirdPersonCamera g_camera;
static PadState g_pad;

int main(int argc, char* argv[]) {
    mem_init(); input_init(); sound_init(); lan_net_init(); renderer_init();
    camera_init(&g_camera); match_init(&g_match);
    while (1) {
        input_poll(&g_pad);
        match_update(&g_match, &g_pad, &g_camera);
        lan_net_broadcast_state(&g_match.player, g_match.frame_counter);
        renderer_draw_frame(&g_match, &g_camera);
        hud_log_telemetry(&g_match);
    }
    return 0;
}
