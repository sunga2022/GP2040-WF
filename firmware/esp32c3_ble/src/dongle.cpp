#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_idf_version.h>
#include "USB.h"
#include "USBHIDGamepad.h"
#include "protocol.h"
#include "hid_pack.h"

/*
 * 2.4G USB dongle: ESP-NOW receive → USB FS HID gamepad.
 * Board: ESP32-S3 (C3 USB cannot be HID with this Arduino core).
 * Plug into the PC. This is a 2.4G receiver, not a Wi-Fi AP.
 */

USBHIDGamepad usbGamepad;
static WfFrame latest;
static volatile bool hasFrame;

#if ESP_IDF_VERSION_MAJOR >= 5
static void onRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    (void)info;
#else
static void onRecv(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
#endif
    WfFrame frame;

    if (len < (int)sizeof(WfFrame)) {
        return;
    }
    memcpy(&frame, data, sizeof(frame));
    if (!wf_frame_valid(&frame)) {
        return;
    }
    latest = frame;
    hasFrame = true;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_wifi_set_channel(WF_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_now_init();
    esp_now_register_recv_cb(onRecv);

    usbGamepad.begin();
    USB.begin();
}

void loop()
{
    WfFrame frame;

    if (!hasFrame) {
        return;
    }
    noInterrupts();
    frame = latest;
    hasFrame = false;
    interrupts();
    digitalWrite(LED_BUILTIN, frame.buttons || frame.dpad);
    usbGamepad.send(0, 0, 0, 0, 0, 0, wf_hat_arduino(frame.dpad), frame.buttons);
}
