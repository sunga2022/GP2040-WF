/*
 * Drop this into the Jieli AC63 HID demo (apps/hid).
 *
 * Switch Pro over Classic BT (no crypto). Xbox/PS stay on the stick USB
 * with GP2040-CE passthrough auth.
 *
 * 1. Add GP2040-WF `wireless/` to the include path.
 * 2. Add gp2040wf_uart_hid.c and wireless/switch_pro.c to the project.
 * 3. Device name "Pro Controller", VID 0x057E, PID 0x2009, EDR HID.
 *    HID report descriptor: wireless/switch_pro_desc.h
 * 4. UART 1 Mbps 8N1: Pico TX -> AC632N RX. ISR: wf_jieli_on_byte.
 *    HID output callback: wf_jieli_on_hid_output.
 *    Main loop: wf_jieli_poll().
 *
 * Official SDK: https://github.com/Jieli-Tech/fw-AC63_BT_SDK
 * Enable TCFG_USER_BLE_ENABLE and TCFG_USER_EDR_ENABLE.
 */

#include "gp2040wf_uart_hid.h"
#include "uart_rx.h"
#include "hid_pack.h"
#include "switch_pro.h"

#ifdef JL_AC63_SDK
#include "app_config.h"
extern int hid_send(uint8_t *data, uint16_t len);
#else
int (*wf_jieli_hid_send)(uint8_t *data, uint16_t len);

static int hid_send(uint8_t *data, uint16_t len)
{
    if (wf_jieli_hid_send) {
        return wf_jieli_hid_send(data, len);
    }
    (void)data;
    (void)len;
    return 0;
}
#endif

static WfUartRx rx;
static int switch_mode;

void wf_jieli_reset(void)
{
    wf_uart_rx_reset(&rx);
    switch_mode = 0;
    wf_switch_pro_init(WF_SWITCH_BT);
}

void wf_jieli_on_hid_output(uint8_t report_id, const uint8_t *data, uint16_t len)
{
    wf_switch_pro_on_output(report_id, data, len);
}

void wf_jieli_poll(void)
{
    uint8_t report[WF_SWITCH_PRO_REPORT_SIZE];
    if (!switch_mode) {
        return;
    }
    if (wf_switch_pro_next_report(report)) {
        hid_send(report, sizeof(report));
    }
}

void wf_jieli_on_byte(uint8_t b)
{
    WfFrame frame;
    uint8_t hid[8];
    uint8_t report[WF_SWITCH_PRO_REPORT_SIZE];

    if (!wf_uart_rx_byte(&rx, b, &frame)) {
        return;
    }
    switch_mode = (wf_flags_mode(frame.flags) == WF_MODE_SWITCH);
    if (switch_mode) {
        wf_switch_pro_apply_frame(&frame);
        if (wf_switch_pro_next_report(report)) {
            hid_send(report, sizeof(report));
        }
        return;
    }
    wf_pack_hid8(&frame, hid);
    hid_send(hid, sizeof(hid));
}
