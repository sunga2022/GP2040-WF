/*
 * GP2040-WF controller coprocessor
 * Board: Seeed XIAO nRF52840 (or nRF52840 DK)
 *
 * UART from RP2040 (1 Mbps) -> 2.4 GHz proprietary radio + BLE HID.
 * User button toggles BLE advertising.
 *
 * Wiring (XIAO nRF52840):
 *   RP2040 GP24 (UART1 TX) -> XIAO D7 / RX
 *   GND                    -> GND
 *
 * Arduino: board "Seeed nRF52 Boards" / Adafruit nRF52, USB stack TinyUSB.
 */

#include <bluefruit.h>
#include "protocol.h"
#include "radio_link.h"

#ifndef PIN_SERIAL1_RX
#define PIN_SERIAL1_RX 7
#endif

BLEDis bledis;
BLEHidAdafruit blehid;
uint8_t hidButtons[2];
int8_t hidX, hidY, hidZ, hidRz;
uint8_t hidHat;
bool bleOn = true;

static uint8_t hat_from_dpad(uint8_t dpad)
{
    const uint8_t up = dpad & 0x01;
    const uint8_t down = dpad & 0x02;
    const uint8_t left = dpad & 0x04;
    const uint8_t right = dpad & 0x08;
    if (up && right) return 2;
    if (right && down) return 4;
    if (down && left) return 6;
    if (left && up) return 8;
    if (up) return 1;
    if (right) return 3;
    if (down) return 5;
    if (left) return 7;
    return 0;
}

static void apply_frame(const WfFrame *frame)
{
    hidHat = hat_from_dpad(frame->dpad);
    hidButtons[0] = (uint8_t)(frame->buttons & 0xFF);
    hidButtons[1] = (uint8_t)((frame->buttons >> 8) & 0xFF);
    hidX = (int8_t)((int16_t)(frame->lx - 0x7FFF) / 256);
    hidY = (int8_t)((int16_t)(frame->ly - 0x7FFF) / 256);
    hidZ = (int8_t)((int16_t)(frame->rx - 0x7FFF) / 256);
    hidRz = (int8_t)((int16_t)(frame->ry - 0x7FFF) / 256);
    wf_radio_send((const uint8_t *)frame, sizeof(*frame));
    if (bleOn && Bluefruit.connected()) {
        blehid.gamepadReport(hidHat, hidButtons, hidX, hidY, hidZ, hidRz);
    }
}

void setup()
{
    Serial1.begin(WF_UART_BAUD);
    wf_radio_init(1);

    Bluefruit.begin();
    Bluefruit.setTxPower(4);
    Bluefruit.setName("GP2040-WF");
    bledis.setManufacturer("sunga2022");
    bledis.setModel("GP2040-WF");
    bledis.begin();
    blehid.begin();
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addService(blehid);
    Bluefruit.Advertising.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.start(0);

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    static uint8_t buf[WF_FRAME_SIZE];
    static uint8_t n = 0;

    while (Serial1.available()) {
        uint8_t b = (uint8_t)Serial1.read();
        if (n == 0 && b != WF_MAGIC0) {
            continue;
        }
        if (n == 1 && b != WF_MAGIC1) {
            n = 0;
            continue;
        }
        buf[n++] = b;
        if (n >= WF_FRAME_SIZE) {
            WfFrame frame;
            memcpy(&frame, buf, sizeof(frame));
            if (wf_frame_valid(&frame)) {
                apply_frame(&frame);
                digitalWrite(LED_BUILTIN, frame.buttons || frame.dpad);
            }
            n = 0;
        }
    }
}
