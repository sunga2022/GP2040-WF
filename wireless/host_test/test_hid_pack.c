#include "hid_pack.h"
#include <stdio.h>
#include <stdlib.h>

static void fail(const char *msg)
{
    fprintf(stderr, "FAIL %s\n", msg);
    exit(1);
}

int main(void)
{
    WfFrame frame;
    uint8_t report[8];

    memset(&frame, 0, sizeof(frame));
    frame.dpad = 0x01; /* up */
    frame.buttons = 0x0005u;
    frame.lx = 0x7FFFu;
    wf_pack_hid8(&frame, report);
    if (report[0] != 5 || report[2] != 0) {
        fail("up");
    }
    if (wf_hat_arduino(0x01) != 1) {
        fail("arduino up");
    }
    if (wf_hat_arduino(0) != 0) {
        fail("arduino center");
    }
    printf("ok hid_pack\n");
    return 0;
}
