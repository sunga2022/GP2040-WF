#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "USB.h"
#include "USBHIDGamepad.h"
#include "protocol.h"
#include "wf_pins.h"

#if WF_ENABLE_BLE
#include <BleGamepad.h>
BleGamepad bleGamepad("GP2040-WF", "sunga2022", 100);
#endif

USBHIDGamepad usbGamepad;

static const int kButtonPins[] = {
    PIN_UP, PIN_DOWN, PIN_LEFT, PIN_RIGHT,
    PIN_B1, PIN_B2, PIN_B3, PIN_B4,
    PIN_L1, PIN_R1, PIN_L2, PIN_R2,
    PIN_S1, PIN_S2, PIN_L3, PIN_R3,
    PIN_A1, PIN_A2,
};
static const int kPinCount = sizeof(kButtonPins) / sizeof(kButtonPins[0]);

static uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t seq;

static uint8_t hatFromDpad(uint8_t dpad)
{
    const bool u = dpad & DPAD_UP;
    const bool d = dpad & DPAD_DOWN;
    const bool l = dpad & DPAD_LEFT;
    const bool r = dpad & DPAD_RIGHT;
    if (u && r) return 2;
    if (r && d) return 4;
    if (d && l) return 6;
    if (l && u) return 8;
    if (u) return 1;
    if (r) return 3;
    if (d) return 5;
    if (l) return 7;
    return 0;
}

static void readButtons(uint8_t *dpad, uint32_t *buttons)
{
    *dpad = 0;
    *buttons = 0;
    if (!digitalRead(PIN_UP)) *dpad |= DPAD_UP;
    if (!digitalRead(PIN_DOWN)) *dpad |= DPAD_DOWN;
    if (!digitalRead(PIN_LEFT)) *dpad |= DPAD_LEFT;
    if (!digitalRead(PIN_RIGHT)) *dpad |= DPAD_RIGHT;
    if (!digitalRead(PIN_B1)) *buttons |= BTN_B1;
    if (!digitalRead(PIN_B2)) *buttons |= BTN_B2;
    if (!digitalRead(PIN_B3)) *buttons |= BTN_B3;
    if (!digitalRead(PIN_B4)) *buttons |= BTN_B4;
    if (!digitalRead(PIN_L1)) *buttons |= BTN_L1;
    if (!digitalRead(PIN_R1)) *buttons |= BTN_R1;
    if (!digitalRead(PIN_L2)) *buttons |= BTN_L2;
    if (!digitalRead(PIN_R2)) *buttons |= BTN_R2;
    if (!digitalRead(PIN_S1)) *buttons |= BTN_S1;
    if (!digitalRead(PIN_S2)) *buttons |= BTN_S2;
    if (!digitalRead(PIN_L3)) *buttons |= BTN_L3;
    if (!digitalRead(PIN_R3)) *buttons |= BTN_R3;
    if (!digitalRead(PIN_A1)) *buttons |= BTN_A1;
    if (!digitalRead(PIN_A2)) *buttons |= BTN_A2;
}

static void sendUsb(uint8_t dpad, uint32_t buttons)
{
    usbGamepad.send(0, 0, 0, 0, 0, 0, hatFromDpad(dpad), buttons);
}

static void sendEspNow(uint8_t dpad, uint32_t buttons)
{
    WfFrame frame;
    memset(&frame, 0, sizeof(frame));
    frame.seq = seq++;
    frame.dpad = dpad;
    frame.buttons = buttons;
    frame.lx = 0x7FFF;
    frame.ly = 0x7FFF;
    frame.rx = 0x7FFF;
    frame.ry = 0x7FFF;
    frame.flags = WF_LINK_24G;
    wf_frame_seal(&frame);
    esp_now_send(broadcast, (uint8_t *)&frame, sizeof(frame));
}

#if WF_ENABLE_BLE
static void sendBle(uint8_t dpad, uint32_t buttons)
{
    if (!bleGamepad.isConnected()) {
        return;
    }
    switch (hatFromDpad(dpad)) {
        case 1: bleGamepad.setHat1(HAT_UP); break;
        case 2: bleGamepad.setHat1(HAT_UP_RIGHT); break;
        case 3: bleGamepad.setHat1(HAT_RIGHT); break;
        case 4: bleGamepad.setHat1(HAT_DOWN_RIGHT); break;
        case 5: bleGamepad.setHat1(HAT_DOWN); break;
        case 6: bleGamepad.setHat1(HAT_DOWN_LEFT); break;
        case 7: bleGamepad.setHat1(HAT_LEFT); break;
        case 8: bleGamepad.setHat1(HAT_UP_LEFT); break;
        default: bleGamepad.setHat1(HAT_CENTERED); break;
    }
    for (uint8_t i = 0; i < 16; i++) {
        if (buttons & (1u << i)) {
            bleGamepad.press(i + 1);
        } else {
            bleGamepad.release(i + 1);
        }
    }
    bleGamepad.sendReport();
}
#endif

void setup()
{
    for (int i = 0; i < kPinCount; i++) {
        pinMode(kButtonPins[i], INPUT_PULLUP);
    }
    pinMode(PIN_LED, OUTPUT);

    WiFi.mode(WIFI_STA);
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_now_init();
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, broadcast, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    usbGamepad.begin();
    USB.begin();

#if WF_ENABLE_BLE
    BleGamepadConfiguration cfg;
    cfg.setAutoReport(false);
    cfg.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    cfg.setButtonCount(16);
    cfg.setHatSwitchCount(1);
    bleGamepad.begin(&cfg);
#endif
}

void loop()
{
    static uint32_t lastUs;
    const uint32_t now = micros();
    if ((uint32_t)(now - lastUs) < 1000) {
        return;
    }
    lastUs = now;

    uint8_t dpad = 0;
    uint32_t buttons = 0;
    readButtons(&dpad, &buttons);
    digitalWrite(PIN_LED, (dpad || buttons) ? HIGH : LOW);

    sendUsb(dpad, buttons);
    sendEspNow(dpad, buttons);
#if WF_ENABLE_BLE
    sendBle(dpad, buttons);
#endif
}
