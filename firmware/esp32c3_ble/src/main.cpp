#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include "protocol.h"
#include "uart_rx.h"

/*
 * External BLE module for GP2040-WF.
 *
 * M487 is the gamepad brain (USB HS 8 kHz on CON1). This chip only does BLE HID.
 *
 * Wiring (ESP32-C3 SuperMini / C3-12F / DevKitM-1):
 *
 *   M487 D1  PB.3 UART1_TXD  ->  GPIO4  (this RX)
 *   M487 D0  PB.2 UART1_RXD  <-  GPIO5  (this TX, optional)
 *   M487 PA.11 USB_ACTIVE    ->  GPIO2  (high = CON1 enumerated, mute BLE)
 *   3V3                      ->  3V3
 *   GND                      ->  GND
 *
 * BLE HID is ~7.5 ms (≈133 Hz). It is not 8 kHz.
 */

#define PIN_RX          4
#define PIN_TX          5
#define PIN_USB_ACTIVE  2
#define PIN_LED         8

static WfUartRx rx;
static NimBLEHIDDevice *hid;
static NimBLECharacteristic *input;
static volatile bool ble_connected;

/* Same 8-byte gamepad as M487, plus Report ID 1 for BLE HID. */
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
        /* 6 * 1.25 ms = 7.5 ms, the BLE HID floor. */
        server->updateConnParams(desc->conn_handle, 6, 6, 0, 100);
    }

    void onDisconnect(NimBLEServer *server) override
    {
        ble_connected = false;
        NimBLEDevice::startAdvertising();
    }
};

static uint8_t hat_from_dpad(uint8_t dpad)
{
    const int u = (dpad & 0x01) != 0;
    const int d = (dpad & 0x02) != 0;
    const int l = (dpad & 0x04) != 0;
    const int r = (dpad & 0x08) != 0;
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

static void pack_hid(const WfFrame *frame, uint8_t report[8])
{
    report[0] = (uint8_t)(frame->buttons & 0xFFu);
    report[1] = (uint8_t)((frame->buttons >> 8) & 0xFFu);
    report[2] = hat_from_dpad(frame->dpad) & 0x0Fu;
    report[3] = 0x80;
    report[4] = 0x80;
    report[5] = 0x80;
    report[6] = 0x80;
    report[7] = 0;
}

static void send_ble(const WfFrame *frame)
{
    uint8_t report[8];

    if (digitalRead(PIN_USB_ACTIVE) == HIGH) {
        return;
    }
    if (!ble_connected || input == nullptr) {
        return;
    }
    pack_hid(frame, report);
    input->setValue(report, sizeof(report));
    input->notify();
}

void setup()
{
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_USB_ACTIVE, INPUT_PULLDOWN);
    digitalWrite(PIN_LED, LOW);

    Serial1.begin(WF_UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
    wf_uart_rx_reset(&rx);

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
        send_ble(&frame);
    }
}
