#ifndef GP2040_WF_HID_PACK_H_
#define GP2040_WF_HID_PACK_H_

#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HID hat: 0=up … 7=up-left, 8=neutral. Same as M487 USB report. */
static inline uint8_t wf_hat_from_dpad(uint8_t dpad)
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

/* Arduino USBHIDGamepad hat: 0=center, 1=up, 2=up-right, … */
static inline uint8_t wf_hat_arduino(uint8_t dpad)
{
    uint8_t hat = wf_hat_from_dpad(dpad);
    return (hat == 8u) ? 0u : (uint8_t)(hat + 1u);
}

static inline void wf_pack_hid8(const WfFrame *frame, uint8_t report[8])
{
    report[0] = (uint8_t)(frame->buttons & 0xFFu);
    report[1] = (uint8_t)((frame->buttons >> 8) & 0xFFu);
    report[2] = wf_hat_from_dpad(frame->dpad) & 0x0Fu;
    report[3] = 0x80;
    report[4] = 0x80;
    report[5] = 0x80;
    report[6] = 0x80;
    report[7] = 0;
}

#ifdef __cplusplus
}
#endif

#endif
