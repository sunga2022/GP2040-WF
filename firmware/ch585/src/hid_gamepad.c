#include "hid_gamepad.h"
#include "wf_pins.h"

void MCU_Sleep_Wakeup_Operate(void)
{
}

uint8_t Gamepad_HatFromDpad(uint8_t dpad)
{
    const int u = (dpad & DPAD_UP) != 0;
    const int d = (dpad & DPAD_DOWN) != 0;
    const int l = (dpad & DPAD_LEFT) != 0;
    const int r = (dpad & DPAD_RIGHT) != 0;

    if (u && r) {
        return 1;
    }
    if (r && d) {
        return 3;
    }
    if (d && l) {
        return 5;
    }
    if (l && u) {
        return 7;
    }
    if (u) {
        return 0;
    }
    if (r) {
        return 2;
    }
    if (d) {
        return 4;
    }
    if (l) {
        return 6;
    }
    return 8;
}

void Gamepad_GpioInit(void)
{
    GPIOB_ModeCfg(PIN_UP | PIN_DOWN | PIN_LEFT | PIN_RIGHT |
                      PIN_B1 | PIN_B2 | PIN_B3 | PIN_B4 |
                      PIN_L1 | PIN_R1 | PIN_L2 | PIN_R2 |
                      PIN_S1 | PIN_S2 | PIN_L3 | PIN_R3,
                  GPIO_ModeIN_PU);
    GPIOA_ModeCfg(PIN_A1 | PIN_A2 | PIN_BIND, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(PIN_LED, GPIO_ModeOut_PP_5mA);
    GPIOA_ResetBits(PIN_LED);
}

void Gamepad_Read(WfPadState *state)
{
    uint8_t dpad = 0;
    uint32_t buttons = 0;

    if (pin_pressed_b(PIN_UP)) {
        dpad |= DPAD_UP;
    }
    if (pin_pressed_b(PIN_DOWN)) {
        dpad |= DPAD_DOWN;
    }
    if (pin_pressed_b(PIN_LEFT)) {
        dpad |= DPAD_LEFT;
    }
    if (pin_pressed_b(PIN_RIGHT)) {
        dpad |= DPAD_RIGHT;
    }
    if (pin_pressed_b(PIN_B1)) {
        buttons |= BTN_B1;
    }
    if (pin_pressed_b(PIN_B2)) {
        buttons |= BTN_B2;
    }
    if (pin_pressed_b(PIN_B3)) {
        buttons |= BTN_B3;
    }
    if (pin_pressed_b(PIN_B4)) {
        buttons |= BTN_B4;
    }
    if (pin_pressed_b(PIN_L1)) {
        buttons |= BTN_L1;
    }
    if (pin_pressed_b(PIN_R1)) {
        buttons |= BTN_R1;
    }
    if (pin_pressed_b(PIN_L2)) {
        buttons |= BTN_L2;
    }
    if (pin_pressed_b(PIN_R2)) {
        buttons |= BTN_R2;
    }
    if (pin_pressed_b(PIN_S1)) {
        buttons |= BTN_S1;
    }
    if (pin_pressed_b(PIN_S2)) {
        buttons |= BTN_S2;
    }
    if (pin_pressed_b(PIN_L3)) {
        buttons |= BTN_L3;
    }
    if (pin_pressed_b(PIN_R3)) {
        buttons |= BTN_R3;
    }
    if (pin_pressed_a(PIN_A1)) {
        buttons |= BTN_A1;
    }
    if (pin_pressed_a(PIN_A2)) {
        buttons |= BTN_A2;
    }

    state->dpad = dpad;
    state->buttons = buttons;
}

void Gamepad_PackHid(const WfPadState *state, uint8_t report[8])
{
    report[0] = (uint8_t)(state->buttons & 0xFFu);
    report[1] = (uint8_t)((state->buttons >> 8) & 0xFFu);
    report[2] = Gamepad_HatFromDpad(state->dpad) & 0x0Fu;
    report[3] = 0x80;
    report[4] = 0x80;
    report[5] = 0x80;
    report[6] = 0x80;
    report[7] = 0;
}
