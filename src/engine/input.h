#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H
#include <tamtypes.h>
#include <libpad.h>
typedef struct { u16 buttons_held, buttons_pressed; float left_stick_x, left_stick_y, right_stick_x, right_stick_y; } PadState;
void input_init(void);
void input_poll(PadState* out_state);
#endif
