#include "hid_gamepad.h"
#include "wf_pins.h"

static void input_pin(GPIO_T *port, uint32_t mask)
{
    GPIO_SetMode(port, mask, GPIO_MODE_INPUT);
    GPIO_SetPullCtl(port, mask, GPIO_PUSEL_PULL_UP);
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
    input_pin(PC, BIT9 | BIT10 | BIT11 | BIT12);
    input_pin(PE, BIT4 | BIT5);
    input_pin(PA, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    input_pin(PB, BIT0 | BIT1 | BIT6 | BIT7 | BIT8 | BIT9);
    input_pin(PF, BIT11);

    GPIO_SetMode(PH, BIT0, GPIO_MODE_OUTPUT);
    PIN_LED = 1; /* active-low LED off */
}

void Gamepad_Read(WfPadState *state)
{
    uint8_t dpad = 0;
    uint32_t buttons = 0;

    if (pin_pressed(PIN_UP_PORT, PIN_UP_BIT)) {
        dpad |= DPAD_UP;
    }
    if (pin_pressed(PIN_DOWN_PORT, PIN_DOWN_BIT)) {
        dpad |= DPAD_DOWN;
    }
    if (pin_pressed(PIN_LEFT_PORT, PIN_LEFT_BIT)) {
        dpad |= DPAD_LEFT;
    }
    if (pin_pressed(PIN_RIGHT_PORT, PIN_RIGHT_BIT)) {
        dpad |= DPAD_RIGHT;
    }
    if (pin_pressed(PIN_B1_PORT, PIN_B1_BIT)) {
        buttons |= BTN_B1;
    }
    if (pin_pressed(PIN_B2_PORT, PIN_B2_BIT)) {
        buttons |= BTN_B2;
    }
    if (pin_pressed(PIN_B3_PORT, PIN_B3_BIT)) {
        buttons |= BTN_B3;
    }
    if (pin_pressed(PIN_B4_PORT, PIN_B4_BIT)) {
        buttons |= BTN_B4;
    }
    if (pin_pressed(PIN_L1_PORT, PIN_L1_BIT)) {
        buttons |= BTN_L1;
    }
    if (pin_pressed(PIN_R1_PORT, PIN_R1_BIT)) {
        buttons |= BTN_R1;
    }
    if (pin_pressed(PIN_L2_PORT, PIN_L2_BIT)) {
        buttons |= BTN_L2;
    }
    if (pin_pressed(PIN_R2_PORT, PIN_R2_BIT)) {
        buttons |= BTN_R2;
    }
    if (pin_pressed(PIN_S1_PORT, PIN_S1_BIT)) {
        buttons |= BTN_S1;
    }
    if (pin_pressed(PIN_S2_PORT, PIN_S2_BIT)) {
        buttons |= BTN_S2;
    }
    if (pin_pressed(PIN_L3_PORT, PIN_L3_BIT)) {
        buttons |= BTN_L3;
    }
    if (pin_pressed(PIN_R3_PORT, PIN_R3_BIT)) {
        buttons |= BTN_R3;
    }
    if (pin_pressed(PIN_A1_PORT, PIN_A1_BIT)) {
        buttons |= BTN_A1;
    }
    if (pin_pressed(PIN_A2_PORT, PIN_A2_BIT)) {
        buttons |= BTN_A2;
    }
    if (pin_pressed(PIN_USER_PORT, PIN_USER_BIT)) {
        buttons |= BTN_S2;
    }

    state->dpad = dpad;
    state->buttons = buttons;
}

void Gamepad_PackHid(const WfPadState *state, uint8_t report[HID_REPORT_SIZE])
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
