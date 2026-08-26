/*
 * GP2040-WF 手柄 —— 沁恒 CH585
 *
 * 插 USB HS：有线 HID，bInterval=1 → 8 kHz。
 * 没插电脑时：2.4G 发出 WfFrame（需 WF_ENABLE_RF=1 + 沁恒 RF ROM）。
 */

#include "CH58x_common.h"
#include "ch585_usbhs_device.h"
#include "hid_gamepad.h"
#include "protocol.h"
#include "rf_link.h"
#include "wf_pins.h"

#include <string.h>

static uint8_t s_seq;

static void send_usb(const WfPadState *state)
{
    uint8_t report[HID_REPORT_SIZE];
    Gamepad_PackHid(state, report);
    USBHS_Endp_DataUp(DEF_UEP1, report, HID_REPORT_SIZE, DEF_UEP_CPY_LOAD);
}

static void send_rf(const WfPadState *state)
{
    WfFrame frame;
    memset(&frame, 0, sizeof(frame));
    frame.seq = s_seq++;
    frame.dpad = state->dpad;
    frame.buttons = state->buttons;
    frame.lx = 0x7FFF;
    frame.ly = 0x7FFF;
    frame.rx = 0x7FFF;
    frame.ry = 0x7FFF;
    frame.flags = WF_LINK_24G;
    wf_frame_seal(&frame);
    rf_link_send(&frame);
}

int main(void)
{
    SetSysClock(SYSCLK_FREQ);
    Gamepad_GpioInit();
    rf_link_init(0);
    USBHS_Device_Init(ENABLE);

    while (1) {
        WfPadState state;
        Gamepad_Read(&state);

        if (USBHS_DevEnumStatus) {
            GPIOA_SetBits(PIN_LED);
            send_usb(&state);
        } else {
            GPIOA_ResetBits(PIN_LED);
            send_rf(&state);
        }
    }
}
