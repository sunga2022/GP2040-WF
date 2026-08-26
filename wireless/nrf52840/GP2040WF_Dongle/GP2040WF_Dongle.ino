/*
 * GP2040-WF 2.4 GHz USB dongle
 * Board: Seeed XIAO nRF52840
 *
 * Receives proprietary 1 Mbps frames and enumerates as a USB HID gamepad
 * at bInterval = 1 (1000 Hz on Full Speed USB — 8 kHz is not possible).
 *
 * Arduino: Adafruit TinyUSB stack enabled.
 */

#include "Adafruit_TinyUSB.h"
#include "protocol.h"
#include "radio_link.h"

uint8_t const desc_hid_report[] = { TUD_HID_REPORT_DESC_GAMEPAD() };
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t report;

static uint8_t hat_from_dpad(uint8_t dpad)
{
    const uint8_t up = dpad & 0x01;
    const uint8_t down = dpad & 0x02;
    const uint8_t left = dpad & 0x04;
    const uint8_t right = dpad & 0x08;
    if (up && right) return GAMEPAD_HAT_UP_RIGHT;
    if (right && down) return GAMEPAD_HAT_DOWN_RIGHT;
    if (down && left) return GAMEPAD_HAT_DOWN_LEFT;
    if (left && up) return GAMEPAD_HAT_UP_LEFT;
    if (up) return GAMEPAD_HAT_UP;
    if (right) return GAMEPAD_HAT_RIGHT;
    if (down) return GAMEPAD_HAT_DOWN;
    if (left) return GAMEPAD_HAT_LEFT;
    return GAMEPAD_HAT_CENTERED;
}

void setup()
{
    TinyUSBDevice.setID(0x1D50, 0x57F6);
    TinyUSBDevice.setProductDescriptor("GP2040-WF Dongle");
    TinyUSBDevice.setManufacturerDescriptor("sunga2022");
    usb_hid.setPollInterval(1);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while (!TinyUSBDevice.mounted()) {
        delay(1);
    }
    wf_radio_init(0);
    memset(&report, 0, sizeof(report));
}

void loop()
{
    uint8_t raw[32];
    if (!wf_radio_recv(raw, sizeof(raw))) {
        return;
    }
    WfFrame frame;
    memcpy(&frame, raw, sizeof(frame));
    if (!wf_frame_valid(&frame)) {
        return;
    }
    report.hat = hat_from_dpad(frame.dpad);
    report.buttons = frame.buttons;
    report.x = (int8_t)((int16_t)(frame.lx - 0x7FFF) / 256);
    report.y = (int8_t)((int16_t)(frame.ly - 0x7FFF) / 256);
    report.z = (int8_t)((int16_t)(frame.rx - 0x7FFF) / 256);
    report.rz = (int8_t)((int16_t)(frame.ry - 0x7FFF) / 256);
    report.rx = (int8_t)(frame.lt / 2);
    report.ry = (int8_t)(frame.rt / 2);
    if (usb_hid.ready()) {
        usb_hid.sendReport(0, &report, sizeof(report));
    }
}
