#include "gp2040wf_uart_hid.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern int (*wf_jieli_hid_send)(uint8_t *data, uint16_t len);

static uint8_t last[64];
static uint16_t last_len;

static int capture(uint8_t *data, uint16_t len)
{
    if (len > sizeof(last)) {
        len = sizeof(last);
    }
    memcpy(last, data, len);
    last_len = len;
    return 0;
}

int main(void)
{
    WfFrame in;
    uint8_t i;

    wf_jieli_hid_send = capture;
    memset(&in, 0, sizeof(in));
    in.seq = 9;
    in.dpad = 0x01;
    in.buttons = 0x0005u;
    in.lx = 0x7FFFu;
    in.ly = 0x7FFFu;
    in.rx = 0x7FFFu;
    in.ry = 0x7FFFu;
    in.flags = wf_flags_pack(WF_LINK_AUTO, WF_MODE_PC);
    wf_frame_seal(&in);

    wf_jieli_reset();
    wf_jieli_on_byte(0x00);
    last_len = 0;
    for (i = 0; i < WF_FRAME_SIZE; i++) {
        wf_jieli_on_byte(((uint8_t *)&in)[i]);
    }
    if (last_len != 8) {
        fprintf(stderr, "FAIL pc hid len %u\n", last_len);
        return 1;
    }

    in.flags = wf_flags_pack(WF_LINK_BLE, WF_MODE_SWITCH);
    wf_frame_seal(&in);
    last_len = 0;
    for (i = 0; i < WF_FRAME_SIZE; i++) {
        wf_jieli_on_byte(((uint8_t *)&in)[i]);
    }
    if (last_len != 64 || last[0] != 0x30) {
        fprintf(stderr, "FAIL switch hid %u id %02x\n", last_len, last[0]);
        return 1;
    }

    printf("ok jieli uart hid glue\n");
    return 0;
}
