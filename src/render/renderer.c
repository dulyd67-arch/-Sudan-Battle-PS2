#include "renderer.h"
#include "../engine/world_stream.h"
#include "../gameplay/vehicles.h"
#include <kernel.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet.h>
#include <graph.h>
#include <draw.h>
static framebuffer_t g_fb[2];
static zbuffer_t g_zb;
static int g_context = 0;
static packet_t* g_packets[2];
static int project_vertex(Vec3 pt, const ThirdPersonCamera* cam, int* sx, int* sy, unsigned int* sz) {
    Vec3 rel = vec3_sub(pt, cam->position);
    float cy = cosf(-cam->yaw), sy = sinf(-cam->yaw), cp = cosf(-cam->pitch), sp = sinf(-cam->pitch);
    float x1 = rel.x * cy + rel.z * sy, z1 = -rel.x * sy + rel.z * cy;
    float y2 = rel.y * cp - z1 * sp, z2 = rel.y * sp + z1 * cp;
    if (z2 < 1.0f || z2 > 420.0f) return 0;
    float fov = 340.0f / z2;
    *sx = (SCREEN_WIDTH / 2) + (int)(x1 * fov);
    *sy = (SCREEN_HEIGHT / 2) - (int)(y2 * fov);
    *sz = (unsigned int)(65535.0f - (z2 * 140.0f));
    return 1;
}
static qword_t* draw_box(qword_t* q, Vec3 center, float r, float h, const ThirdPersonCamera* cam, u8 red, u8 g, u8 b) {
    Vec3 top = {center.x, center.y + h, center.z};
    int sx1, sy1, sx2, sy2; unsigned int sz1, sz2;
    if (!project_vertex(center, cam, &sx1, &sy1, &sz1) || !project_vertex(top, cam, &sx2, &sy2, &sz2)) return q;
    int half_w = (int)((sy1 - sy2) * (r / (h + 0.01f)));
    if (half_w < 2) half_w = 2;
    rect_t rc = {{sx1 - half_w, sy2, (u16)sz1}, {sx1 + half_w, sy1}, {red, g, b, 0x80, 1.0f}};
    return draw_rect_filled(q, 0, &rc);
}
void renderer_init(void) {
    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);
    for (int i = 0; i < 2; i++) {
        g_fb[i] = (framebuffer_t){SCREEN_WIDTH, SCREEN_HEIGHT, 0, GS_PSM_24, graph_vram_allocate(SCREEN_WIDTH, SCREEN_HEIGHT, GS_PSM_24, GRAPH_ALIGN_PAGE)};
        g_packets[i] = packet_init(16384, PACKET_NORMAL);
    }
    g_zb = (zbuffer_t){1, 0, ZTEST_METHOD_GREATER_EQUAL, GS_ZBUF_24, graph_vram_allocate(SCREEN_WIDTH, SCREEN_HEIGHT, GS_ZBUF_24, GRAPH_ALIGN_PAGE)};
    graph_initialize(g_fb[0].address, SCREEN_WIDTH, SCREEN_HEIGHT, GS_PSM_24, 0, 0);
}
void renderer_draw_frame(const BattleRoyaleMatch* m, const ThirdPersonCamera* cam) {
    packet_t* pkt = g_packets[g_context];
    qword_t* q = pkt->data;
    q = draw_setup_environment(q, 0, &g_fb[g_context], &g_zb);
    q = draw_primitive_xyoffset(q, 0, 2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    q = draw_clear(q, 0, 2048.0f - (SCREEN_WIDTH / 2), 2048.0f - (SCREEN_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT, 212, 178, 125);
    int sec_count = 0;
    const WorldSector* secs = world_stream_get_sectors(&sec_count);
    for (int s = 0; s < sec_count; s++) {
        if (!secs[s].is_loaded) continue;
        for (int b = 0; b < secs[s].building_count; b++) {
            AABB box = secs[s].buildings[b];
            Vec3 c = {(box.min.x + box.max.x) * 0.5f, 0.0f, (box.min.z + box.max.z) * 0.5f};
            if (camera_frustum_contains_point(cam, c, 340.0f)) q = draw_box(q, c, 12.0f, box.max.y, cam, 165, 130, 95);
        }
    }
    for (int i = 0; i < MAX_LOOT_ITEMS; i++)
        if (m->loot[i].active && camera_frustum_contains_point(cam, m->loot[i].pos, 180.0f))
            q = draw_box(q, m->loot[i].pos, 0.9f, 1.0f, cam, 40, 200, 75);
    int v_count = 0; const Vehicle* vehs = vehicles_get_all(&v_count);
    for (int i = 0; i < v_count; i++)
        if (camera_frustum_contains_point(cam, vehs[i].pos, 250.0f))
            q = draw_box(q, vehs[i].pos, 2.4f, 2.2f, cam, 80, 110, 160);
    for (int i = 0; i < MAX_BOTS; i++)
        if (m->bots[i].alive && camera_frustum_contains_point(cam, m->bots[i].pos, 220.0f))
            q = draw_box(q, m->bots[i].pos, 0.7f, 1.9f, cam, 200, 45, 45);
    if (m->player.alive) {
        float h = (m->player.stance == 2) ? 0.7f : ((m->player.stance == 1) ? 1.3f : 1.9f);
        q = draw_box(q, m->player.pos, 0.65f, h, cam, 40, 95, 45);
    }
    rect_t hp = {{24, 400, 0xFFFF}, {24 + (m->player.health * 2), 414}, {40, 210, 60, 0x80, 1.0f}};
    q = draw_rect_filled(q, 0, &hp);
    q = draw_finish(q);
    dma_wait_fast();
    dma_channel_send_normal(DMA_CHANNEL_GIF, pkt->data, q - pkt->data, 0, 0);
    draw_wait_finish();
    graph_wait_vsync();
    graph_set_framebuffer_filtered(g_fb[g_context].address, SCREEN_WIDTH, GS_PSM_24, 0, 0);
    g_context ^= 1;
}
