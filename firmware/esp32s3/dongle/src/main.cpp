#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "USB.h"
#include "USBHIDGamepad.h"
#include <esp_idf_version.h>
#include "protocol.h"

USBHIDGamepad usbGamepad;
static WfFrame latest;
static volatile bool hasFrame;

static uint8_t hatFromDpad(uint8_t dpad)
{
    const bool u = dpad & 0x01;
    const bool d = dpad & 0x02;
    const bool l = dpad & 0x04;
    const bool r = dpad & 0x08;
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

#if ESP_IDF_VERSION_MAJOR >= 5
static void onRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    (void)info;
#else
static void onRecv(const uint8_t *mac, const uint8_t *data, int len)
{
    (void)mac;
#endif
    if (len < (int)sizeof(WfFrame)) {
        return;
    }
    WfFrame frame;
    memcpy(&frame, data, sizeof(frame));
    if (!wf_frame_valid(&frame)) {
        return;
    }
    latest = frame;
    hasFrame = true;
}

void setup()
{
    WiFi.mode(WIFI_STA);
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_now_init();
    esp_now_register_recv_cb(onRecv);

    usbGamepad.begin();
    USB.begin();
}

void loop()
{
    if (!hasFrame) {
        return;
    }
    noInterrupts();
    WfFrame frame = latest;
    hasFrame = false;
    interrupts();
    usbGamepad.send(0, 0, 0, 0, 0, 0, hatFromDpad(frame.dpad), frame.buttons);
}
