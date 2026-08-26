#include "addons/wireless.h"
#include "protocol.h"
#include "nrf24.h"
#include "helper.h"
#include "hardware/gpio.h"
#include "tusb.h"
#include <string.h>

bool WirelessAddon::available() {
    if (!WIRELESS_ENABLED) {
        return false;
    }
    return uartReady() || nrfPinsReady();
}

bool WirelessAddon::uartReady() const {
    return isValidPin(WIRELESS_UART_TX_PIN);
}

bool WirelessAddon::nrfPinsReady() const {
    return isValidPin(WIRELESS_NRF24_CE_PIN)
        && isValidPin(WIRELESS_NRF24_CSN_PIN)
        && isValidPin(WIRELESS_NRF24_SCK_PIN)
        && isValidPin(WIRELESS_NRF24_MOSI_PIN);
}

void WirelessAddon::setup() {
    nextSendUs = 0;
    nrf24Ready = false;
    uart = nullptr;

    if (uartReady()) {
        uart = (WIRELESS_UART_ID == 0) ? uart0 : uart1;
        uart_init(uart, WF_UART_BAUD);
        gpio_set_function(WIRELESS_UART_TX_PIN, GPIO_FUNC_UART);
        uart_set_hw_flow(uart, false, false);
        uart_set_format(uart, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(uart, true);
    }

    if (nrfPinsReady()) {
        WfNrf24Pins pins = {
            WIRELESS_NRF24_CE_PIN,
            WIRELESS_NRF24_CSN_PIN,
            WIRELESS_NRF24_SCK_PIN,
            WIRELESS_NRF24_MOSI_PIN,
            WIRELESS_NRF24_MISO_PIN,
        };
        wf_nrf24_init(&pins, 1);
        nrf24Ready = true;
    }
}

void WirelessAddon::process() {
    /* Wired USB wins: do not also spray BLE/2.4G at the same host. */
    if (tud_mounted()) {
        return;
    }

    const uint64_t now = getMicro();
    if (now < nextSendUs) {
        return;
    }
    nextSendUs = now + 1000;

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

    if (uart != nullptr) {
        uart_write_blocking(uart, (const uint8_t *)&frame, sizeof(frame));
    }
    if (nrf24Ready) {
        wf_nrf24_send((const uint8_t *)&frame, sizeof(frame));
    }
}
