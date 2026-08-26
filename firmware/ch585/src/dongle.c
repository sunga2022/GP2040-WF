/*
 * GP2040-WF 2.4G 接收器 —— 沁恒 CH585
 *
 * 插电脑 USB HS：HID 8 kHz。
 * 收到手柄 WfFrame 后转成同一份 HID 报告。
 * 默认 make 没有射频库，接收器会枚举成空闲手柄；开 WF_ENABLE_RF 后才收 2.4G。
 */

#include "CH58x_common.h"
#include "ch585_usbhs_device.h"
#include "hid_gamepad.h"
#include "protocol.h"
#include "rf_link.h"
#include "wf_pins.h"

int main(void)
{
    WfPadState state;

    SetSysClock(SYSCLK_FREQ);
    Gamepad_GpioInit();
    rf_link_init(1);
    USBHS_Device_Init(ENABLE);

    state.dpad = 0;
    state.buttons = 0;

    while (1) {
        WfFrame frame;
        if (rf_link_recv(&frame)) {
            state.dpad = frame.dpad;
            state.buttons = frame.buttons;
            GPIOA_SetBits(PIN_LED);
        }

        if (USBHS_DevEnumStatus) {
            uint8_t report[HID_REPORT_SIZE];
            Gamepad_PackHid(&state, report);
            USBHS_Endp_DataUp(DEF_UEP1, report, HID_REPORT_SIZE, DEF_UEP_CPY_LOAD);
        }
    }
}
