#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "protocol.h"
#include "nrf24.h"
#include "switch_pro.h"
#include <string.h>

#ifndef CE_PIN
#define CE_PIN 1
#endif
#ifndef CSN_PIN
#define CSN_PIN 2
#endif
#ifndef SCK_PIN
#define SCK_PIN 3
#endif
#ifndef MOSI_PIN
#define MOSI_PIN 4
#endif
#ifndef MISO_PIN
#define MISO_PIN 5
#endif

#define HID_KEEPALIVE_US 5000u
#define LINK_TIMEOUT_US  50000u

static uint8_t rxbuf[32];
static uint8_t hidbuf[WF_SWITCH_PRO_REPORT_SIZE];
static uint64_t lastPacketUs;
static uint64_t lastHidUs;
static int havePacket;

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)report_type;
    wf_switch_pro_on_output(report_id, buffer, bufsize);
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

int main(void)
{
    board_init();
    wf_switch_pro_init(WF_SWITCH_USB);

    WfNrf24Pins pins = { CE_PIN, CSN_PIN, SCK_PIN, MOSI_PIN, MISO_PIN };
    wf_nrf24_init(&pins, 0);
    tusb_init();

    while (1) {
        tud_task();
        const uint64_t now = time_us_64();

        if (wf_nrf24_recv(rxbuf, WF_FRAME_SIZE)) {
            WfFrame frame;
            memcpy(&frame, rxbuf, sizeof(frame));
            if (wf_frame_valid(&frame)) {
                wf_switch_pro_apply_frame(&frame);
                lastPacketUs = now;
                havePacket = 1;
            }
        }

        if (havePacket && (now - lastPacketUs) > LINK_TIMEOUT_US) {
            WfFrame idle;
            memset(&idle, 0, sizeof(idle));
            idle.lx = idle.ly = idle.rx = idle.ry = 0x7FFF;
            wf_switch_pro_apply_frame(&idle);
            havePacket = 0;
        }

        if (tud_hid_ready() && (now - lastHidUs) >= HID_KEEPALIVE_US) {
            if (wf_switch_pro_next_report(hidbuf)) {
                tud_hid_report(0, hidbuf, sizeof(hidbuf));
            }
            lastHidUs = now;
        }
    }
}
