/*
 * Drop this into the Jieli AC63 HID demo (apps/hid).
 *
 * 1. Add the GP2040-WF `wireless/` directory to the include path
 *    (needs protocol.h, uart_rx.h, hid_pack.h).
 * 2. In the UART RX ISR / byte callback, call wf_jieli_on_byte(byte).
 * 3. UART: 1 000 000 baud, 8N1. Pico TX -> AC632N RX, common GND, 3.3 V.
 *
 * Official SDK: https://github.com/Jieli-Tech/fw-AC63_BT_SDK
 * Project:      apps/hid/board/bd19/AC632N_hid.cbp
 * Enable TCFG_USER_BLE_ENABLE and TCFG_USER_EDR_ENABLE.
 */

#include "gp2040wf_uart_hid.h"
#include "uart_rx.h"
#include "hid_pack.h"

#ifdef JL_AC63_SDK
#include "app_config.h"
extern int hid_send(uint8_t *data, uint16_t len);
#else
static int hid_send(uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;
    return 0;
}
#endif

static WfUartRx rx;

void wf_jieli_reset(void)
{
    wf_uart_rx_reset(&rx);
}

void wf_jieli_on_byte(uint8_t b)
{
    WfFrame frame;
    uint8_t hid[8];

    if (!wf_uart_rx_byte(&rx, b, &frame)) {
        return;
    }
    wf_pack_hid8(&frame, hid);
    hid_send(hid, sizeof(hid));
}
