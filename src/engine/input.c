#include "input.h"
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <string.h>
static char g_pad_buf[256] __attribute__((aligned(64)));
static u16 g_prev_btns = 0;
static int g_pad_ready = 0;
static float normalize_axis(u8 raw) {
    int c = (int)raw - 128;
    return (c > -18 && c < 18) ? 0.0f : (float)c / 127.0f;
}
void input_init(void) {
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);
    padInit(0);
    if (padPortOpen(0, 0, g_pad_buf) > 0) g_pad_ready = 1;
}
void input_poll(PadState* out) {
    memset(out, 0, sizeof(PadState));
    if (!g_pad_ready || padGetState(0, 0) != PAD_STATE_STABLE) return;
    struct padButtonStatus d;
    if (padRead(0, 0, &d) != 0) {
        u16 btns = 0xFFFF ^ d.btns;
        out->buttons_held = btns;
        out->buttons_pressed = btns & ~g_prev_btns;
        g_prev_btns = btns;
        out->left_stick_x = normalize_axis(d.ljoy_h);
        out->left_stick_y = normalize_axis(d.ljoy_v);
        out->right_stick_x = normalize_axis(d.rjoy_h);
        out->right_stick_y = normalize_axis(d.rjoy_v);
    }
}
