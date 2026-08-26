#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "protocol.h"
#include "nrf24.h"
#include "hid_pack.h"
#include <string.h>

#ifndef CE_PIN
#define CE_PIN 1
#endif
#ifndef CSN_PIN
#define CSN_PIN 21
#endif
#ifndef SCK_PIN
#define SCK_PIN 25
#endif
#ifndef MOSI_PIN
#define MOSI_PIN 29
#endif
#ifndef MISO_PIN
#define MISO_PIN 16
#endif

#define HID_KEEPALIVE_US 1000u
#define LINK_TIMEOUT_US  50000u

static hid_gamepad_report_t report;
static uint8_t rxbuf[32];
static uint64_t lastPacketUs;
static uint64_t lastHidUs;
static int havePacket;

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)bufsize;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)reqlen;
    return 0;
}

static void frame_to_report(const WfFrame *frame)
{
    memset(&report, 0, sizeof(report));
    report.buttons = frame->buttons;
    report.hat = wf_hat_arduino(frame->dpad);
    report.x = (int8_t)((int16_t)(frame->lx - 0x7FFF) / 256);
    report.y = (int8_t)((int16_t)(frame->ly - 0x7FFF) / 256);
    report.z = (int8_t)((int16_t)(frame->rx - 0x7FFF) / 256);
    report.rz = (int8_t)((int16_t)(frame->ry - 0x7FFF) / 256);
    report.rx = (int8_t)(frame->lt - 128);
    report.ry = (int8_t)(frame->rt - 128);
}

int main(void)
{
    board_init();

    WfNrf24Pins pins = { CE_PIN, CSN_PIN, SCK_PIN, MOSI_PIN, MISO_PIN };
    wf_nrf24_init(&pins, 0);
    tusb_init();
    memset(&report, 0, sizeof(report));
    report.hat = 0;

    while (1) {
        tud_task();
        const uint64_t now = time_us_64();

        if (wf_nrf24_recv(rxbuf, WF_FRAME_SIZE)) {
            WfFrame frame;
            memcpy(&frame, rxbuf, sizeof(frame));
            if (wf_frame_valid(&frame)) {
                frame_to_report(&frame);
                lastPacketUs = now;
                havePacket = 1;
            }
        }

        if (havePacket && (now - lastPacketUs) > LINK_TIMEOUT_US) {
            memset(&report, 0, sizeof(report));
            havePacket = 0;
        }

        if (tud_hid_ready() && (now - lastHidUs) >= HID_KEEPALIVE_US) {
            tud_hid_report(0, &report, sizeof(report));
            lastHidUs = now;
        }
    }
}
