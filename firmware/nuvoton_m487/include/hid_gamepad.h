#ifndef HID_GAMEPAD_H
#define HID_GAMEPAD_H

#include <stdint.h>

#define HID_REPORT_SIZE  8

typedef struct {
    uint8_t  dpad;
    uint32_t buttons;
} WfPadState;

void Gamepad_GpioInit(void);
void Gamepad_Read(WfPadState *state);
void Gamepad_PackHid(const WfPadState *state, uint8_t report[HID_REPORT_SIZE]);
uint8_t Gamepad_HatFromDpad(uint8_t dpad);

#endif
