#ifndef HID_GAMEPAD_H
#define HID_GAMEPAD_H

#include "stdint.h"

typedef struct {
    uint8_t  dpad;
    uint32_t buttons;
} WfPadState;

void Gamepad_GpioInit(void);
void Gamepad_Read(WfPadState *state);
void Gamepad_PackHid(const WfPadState *state, uint8_t report[8]);
uint8_t Gamepad_HatFromDpad(uint8_t dpad);
void MCU_Sleep_Wakeup_Operate(void);

#endif
