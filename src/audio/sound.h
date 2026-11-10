#ifndef AUDIO_SOUND_H
#define AUDIO_SOUND_H
#include "../common.h"
void sound_init(void);
void sound_play_positional(int id, Vec3 l, Vec3 s);
#endif
