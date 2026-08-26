#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include "protocol.h"
#include "uart_rx.h"
#include "hid_pack.h"

/*
 * Stick-side module: UART from M487 → BLE HID + ESP-NOW 2.4G.
 * Not 802.11 home Wi-Fi. ESP-NOW is the 2.4G dongle link.
 *
 *   M487 D1  PB.3 UART1_TXD  ->  GPIO4  RX
 *   M487 D0  PB.2 UART1_RXD  <-  GPIO5  TX (optional)
 *   M487 PA.11 USB_ACTIVE    ->  GPIO2  high = CON1 enumerated, mute wireless
 *   3V3 / GND
 */

#define PIN_RX          4
#define PIN_TX          5
#define PIN_USB_ACTIVE  2
#define PIN_LED         8

static WfUartRx rx;
static NimBLEHIDDevice *hid;
static NimBLECharacteristic *input;
static volatile bool ble_connected;
static const uint8_t kBroadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const uint8_t kReportMap[] = {
    0x05, 0x01,
    0x09, 0x05,
    0xA1, 0x01,
    0x85, 0x01,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x10,
    0x05, 0x09,
    0x19, 0x01,
    0x29, 0x10,
    0x81, 0x02,
    0x05, 0x01,
    0x09, 0x39,
    0x15, 0x00,
    0x25, 0x07,
    0x35, 0x00,
    0x46, 0x3B, 0x01,
    0x65, 0x14,
    0x75, 0x04,
    0x95, 0x01,
    0x81, 0x42,
    0x65, 0x00,
    0x75, 0x04,
    0x95, 0x01,
    0x81, 0x01,
    0x09, 0x30,
    0x09, 0x31,
    0x09, 0x32,
    0x09, 0x35,
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x04,
    0x81, 0x02,
    0xC0
};

class HidCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *server, ble_gap_conn_desc *desc) override
    {
        ble_connected = true;
        server->updateConnParams(desc->conn_handle, 6, 6, 0, 100);
    }

    void onDisconnect(NimBLEServer *server) override
    {
        (void)server;
        ble_connected = false;
        NimBLEDevice::startAdvertising();
    }
};

static int wireless_muted(void)
{
    return digitalRead(PIN_USB_ACTIVE) == HIGH;
}

static void send_ble(const WfFrame *frame)
{
    uint8_t report[8];

    if (wireless_muted() || !ble_connected || input == nullptr) {
        return;
    }
    wf_pack_hid8(frame, report);
    input->setValue(report, sizeof(report));
    input->notify();
}

static void send_24g(const WfFrame *frame)
{
    WfFrame out;

    if (wireless_muted()) {
        return;
    }
    out = *frame;
    out.flags = WF_LINK_24G;
    wf_frame_seal(&out);
    esp_now_send(kBroadcast, (uint8_t *)&out, sizeof(out));
}

static void radio_init(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_wifi_set_channel(WF_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) {
        return;
    }
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, kBroadcast, 6);
    peer.channel = WF_ESPNOW_CHANNEL;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
}

void setup()
{
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_USB_ACTIVE, INPUT_PULLDOWN);
    digitalWrite(PIN_LED, LOW);

    Serial1.begin(WF_UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
    wf_uart_rx_reset(&rx);

    radio_init();

    NimBLEDevice::init("GP2040-WF");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityAuth(true, false, true);

    NimBLEServer *server = NimBLEDevice::createServer();
    server->setCallbacks(new HidCallbacks());

    hid = new NimBLEHIDDevice(server);
    hid->manufacturer("sunga2022");
    hid->pnp(0x02, 0x0416, 0xB204, 0x0110);
    hid->hidInfo(0x00, 0x01);
    hid->reportMap((uint8_t *)kReportMap, sizeof(kReportMap));
    input = hid->inputReport(1);
    hid->setBatteryLevel(100);
    hid->startServices();

    NimBLEAdvertising *adv = server->getAdvertising();
    adv->setAppearance(HID_GAMEPAD);
    adv->addServiceUUID(hid->hidService()->getUUID());
    adv->setName("GP2040-WF");
    adv->start();
}

void loop()
{
    while (Serial1.available()) {
        WfFrame frame;
        if (!wf_uart_rx_byte(&rx, (uint8_t)Serial1.read(), &frame)) {
            continue;
        }
        digitalWrite(PIN_LED, frame.buttons || frame.dpad);
        send_24g(&frame);
        send_ble(&frame);
    }
}
