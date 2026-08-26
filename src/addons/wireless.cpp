#include "addons/wireless.h"
#include "protocol.h"
#include "helper.h"
#include "hardware/gpio.h"
#include <string.h>

bool WirelessAddon::available() {
    return WIRELESS_ENABLED && isValidPin(WIRELESS_UART_TX_PIN);
}

void WirelessAddon::setup() {
    uart = (WIRELESS_UART_ID == 0) ? uart0 : uart1;
    uart_init(uart, WF_UART_BAUD);
    gpio_set_function(WIRELESS_UART_TX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(uart, false, false);
    uart_set_format(uart, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart, true);
    nextSendUs = 0;
}

void WirelessAddon::process() {
    const uint64_t now = getMicro();
    if (now < nextSendUs) {
        return;
    }
    nextSendUs = now + 1000; // 1000 Hz ceiling; USB Full Speed cannot go faster

    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    if (gamepad == nullptr) {
        return;
    }

    WfFrame frame;
    memset(&frame, 0, sizeof(frame));
    frame.seq = seq++;
    frame.dpad = gamepad->state.dpad;
    frame.buttons = gamepad->state.buttons;
    frame.lx = gamepad->state.lx;
    frame.ly = gamepad->state.ly;
    frame.rx = gamepad->state.rx;
    frame.ry = gamepad->state.ry;
    frame.lt = gamepad->state.lt;
    frame.rt = gamepad->state.rt;
    frame.flags = WF_LINK_AUTO;
    wf_frame_seal(&frame);

    uart_write_blocking(uart, (const uint8_t *)&frame, sizeof(frame));
}
