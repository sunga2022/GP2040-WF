#include "nrf24.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    WfNrf24Pins pins = { 1, 21, 25, 29, -1 };
    uint8_t buf[WF_FRAME_SIZE];
    WfFrame frame;

    memset(&frame, 0, sizeof(frame));
    frame.seq = 1;
    frame.dpad = 1;
    wf_frame_seal(&frame);

    wf_nrf24_init(&pins, 1);
    if (!wf_nrf24_send((const uint8_t *)&frame, sizeof(frame))) {
        fprintf(stderr, "FAIL send\n");
        return 1;
    }
    /* Host stub has no MISO, recv must stay quiet. */
    if (wf_nrf24_recv(buf, sizeof(buf))) {
        fprintf(stderr, "FAIL recv without miso\n");
        return 1;
    }
    printf("ok nrf24 stub  payload=%u\n", (unsigned)WF_FRAME_SIZE);
    return 0;
}
