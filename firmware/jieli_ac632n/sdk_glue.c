/*
 * Glue for fw-AC63_BT_SDK HID (AC632N / bd19).
 * Pico GP24 UART1 TX (1 Mbps) -> this chip UART1 RX (PA5).
 */
#include "app_config.h"
#include "system/includes.h"
#include "asm/uart_dev.h"
#include "generic/gpio.h"
#include "edr_hid_user.h"
#include "gp2040wf_uart_hid.h"

#ifndef WF_JL_UART_RX_PIN
#define WF_JL_UART_RX_PIN   IO_PORTA_05
#endif

#ifndef WF_JL_UART_BAUD
#define WF_JL_UART_BAUD     1000000
#endif

static u8 s_rxbuf[256];
static const uart_bus_t *s_uart;

int hid_send(u8 *data, u16 len)
{
    if (!data || !len) {
        return 0;
    }
    return user_hid_send_data(data, len);
}

void wf_jieli_edr_output(u8 *packet, u16 size, u16 channel)
{
    u8 id = 0;
    const u8 *payload = packet;
    u16 n = size;
    (void)channel;
    if (size >= 1) {
        id = packet[0];
        payload = packet + 1;
        n = (u16)(size - 1);
    }
    wf_jieli_on_hid_output(id, payload, n);
}

static void uart_isr(void *ut_bus, u32 status)
{
    u8 b;
    (void)status;
    if (!s_uart) {
        return;
    }
    while (s_uart->getbyte(&b, 0)) {
        wf_jieli_on_byte(b);
    }
}

static void poll_timer(void *priv)
{
    (void)priv;
    wf_jieli_poll();
}

void wf_jieli_sdk_start(void)
{
    struct uart_platform_data_t pd = {0};

    wf_jieli_reset();

    pd.tx_pin = (u8)-1;
    pd.rx_pin = WF_JL_UART_RX_PIN;
    pd.rx_cbuf = s_rxbuf;
    pd.rx_cbuf_size = sizeof(s_rxbuf);
    pd.frame_length = 1;
    pd.rx_timeout = 4;
    pd.isr_cbfun = uart_isr;
    pd.baud = WF_JL_UART_BAUD;
    s_uart = uart_dev_open(&pd);

    sys_timer_add(NULL, poll_timer, 1);
}
