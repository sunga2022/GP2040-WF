#include "switch_pro.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void fail(const char *msg)
{
    fprintf(stderr, "FAIL %s\n", msg);
    exit(1);
}

int main(void)
{
    uint8_t report[WF_SWITCH_PRO_REPORT_SIZE];
    WfFrame frame;
    uint8_t out[64];

    memset(&frame, 0, sizeof(frame));
    frame.dpad = 0x01; /* up */
    frame.buttons = (1u << 0); /* B1 -> Switch B */
    frame.lx = frame.ly = frame.rx = frame.ry = 0x7FFF;
    wf_frame_seal(&frame);

    wf_switch_pro_init(WF_SWITCH_USB);
    if (!wf_switch_pro_next_report(report) || report[0] != 0x81 || report[1] != 0x01) {
        fail("usb identify");
    }

    memset(out, 0, sizeof(out));
    out[0] = 0x80;
    out[1] = 0x04; /* DISABLE_USB_TIMEOUT */
    wf_switch_pro_on_output(0, out, 2);
    if (!wf_switch_pro_is_ready()) {
        fail("usb ready");
    }
    wf_switch_pro_apply_frame(&frame);
    wf_switch_pro_next_report(report); /* drain handshake reply */
    wf_switch_pro_next_report(report);
    if (report[0] != 0x30) {
        fail("usb 0x30");
    }
    if ((report[3] & 0x04) == 0) { /* B */
        fail("button B");
    }
    if ((report[5] & 0x02) == 0) { /* dpad up */
        fail("dpad up");
    }

    wf_switch_pro_init(WF_SWITCH_BT);
    if (!wf_switch_pro_is_ready()) {
        fail("bt ready");
    }
    memset(out, 0, sizeof(out));
    out[0] = 0x01;
    out[10] = 0x03; /* SET_MODE */
    out[11] = 0x30;
    wf_switch_pro_on_output(0x01, out, 12);
    if (!wf_switch_pro_next_report(report) || report[0] != 0x21 || report[14] != 0x03) {
        fail("set mode ack");
    }

    memset(out, 0, sizeof(out));
    out[0] = 0x01;
    out[10] = 0x02; /* DEVICE_INFO */
    wf_switch_pro_on_output(0x01, out, 11);
    if (!wf_switch_pro_next_report(report) || report[0] != 0x21 || report[13] != 0x82) {
        fail("device info");
    }
    if (report[17] != 0x03) {
        fail("pro type");
    }

    printf("ok switch_pro usb handshake + bt subcommands\n");
    return 0;
}
