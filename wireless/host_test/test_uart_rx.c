/* Host build of the UART frame assembler used by the BLE module. */
#include "uart_rx.h"
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
    WfUartRx rx;
    WfFrame out;
    WfFrame in;
    uint8_t i;

    wf_uart_rx_reset(&rx);
    memset(&in, 0, sizeof(in));
    in.seq = 7;
    in.dpad = 1;
    in.buttons = 0x0003u;
    in.lx = 0x7FFFu;
    in.ly = 0x7FFFu;
    in.rx = 0x7FFFu;
    in.ry = 0x7FFFu;
    in.flags = WF_LINK_BLE;
    wf_frame_seal(&in);

    if (wf_uart_rx_byte(&rx, 0x00, &out)) {
        fail("noise");
    }
    for (i = 0; i < WF_FRAME_SIZE; i++) {
        int done = wf_uart_rx_byte(&rx, ((uint8_t *)&in)[i], &out);
        if (i + 1u < WF_FRAME_SIZE && done) {
            fail("early");
        }
        if (i + 1u == WF_FRAME_SIZE && !done) {
            fail("missing");
        }
    }
    if (out.seq != 7 || out.dpad != 1 || out.buttons != 0x0003u) {
        fail("fields");
    }

    wf_uart_rx_reset(&rx);
    wf_uart_rx_byte(&rx, WF_MAGIC0, &out);
    if (wf_uart_rx_byte(&rx, 0x00, &out)) {
        fail("bad magic1");
    }
    printf("ok uart_rx  %u-byte WfFrame\n", (unsigned)WF_FRAME_SIZE);
    return 0;
}
