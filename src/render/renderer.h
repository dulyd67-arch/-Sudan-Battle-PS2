#ifndef RENDER_RENDERER_H
#define RENDER_RENDERER_H
#include "../gameplay/match.h"
#include "../engine/camera.h"
void renderer_init(void);
void renderer_draw_frame(const BattleRoyaleMatch* match, const ThirdPersonCamera* cam);
#endif
