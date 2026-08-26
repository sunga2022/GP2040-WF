#include "gp2040wf_uart_hid.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
    WfFrame in;
    uint8_t i;

    memset(&in, 0, sizeof(in));
    in.seq = 9;
    in.dpad = 0x01;
    in.buttons = 0x0005u;
    in.lx = 0x7FFFu;
    in.ly = 0x7FFFu;
    in.rx = 0x7FFFu;
    in.ry = 0x7FFFu;
    wf_frame_seal(&in);

    wf_jieli_reset();
    wf_jieli_on_byte(0x00);
    for (i = 0; i < WF_FRAME_SIZE; i++) {
        wf_jieli_on_byte(((uint8_t *)&in)[i]);
    }
    printf("ok jieli uart hid glue\n");
    return 0;
}
