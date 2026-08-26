#include "bt_uart.h"
#include "protocol.h"
#include "wf_pins.h"

/*
 * UART1 @ 1 Mbps on Arduino D0/D1 → stick-side BLE + 2.4G module.
 *
 *   M487 D1 PB.3 UART1_TXD  ->  module RX
 *   M487 D0 PB.2 UART1_RXD  <-  module TX (unused for now)
 *   M487 NU7.7 PA.11        ->  module USB_ACTIVE (high = wired USB, mute BLE and 2.4G)
 *   3V3 / GND
 *
 * BLE cannot do 8 kHz. This UART is paced at 1 kHz.
 */

static volatile uint32_t s_ms;
static uint32_t s_last_ms;
static uint8_t s_seq;

void SysTick_Handler(void)
{
    s_ms++;
}

static void uart1_write(const uint8_t *data, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        while ((UART1->FIFOSTS & UART_FIFOSTS_TXFULL_Msk) != 0u) {
        }
        UART1->DAT = data[i];
    }
}

void BtUart_Init(void)
{
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HXT, CLK_CLKDIV0_UART1(1));

    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB2MFP_UART1_RXD | SYS_GPB_MFPL_PB3MFP_UART1_TXD);

    /* Mode 2: baud = UART_CLK / (BRD + 2). 12 MHz / 12 = 1 Mbps. */
    UART1->FUNCSEL = 0;
    UART1->LINE = 0x3u; /* 8N1 */
    UART1->FIFO = 0;
    UART1->BAUD = UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk | 10u;

    GPIO_SetMode(PA, BIT11, GPIO_MODE_OUTPUT);
    PIN_USB_ACTIVE = 0;

    SysTick_Config(SystemCoreClock / 1000u);
    s_last_ms = s_ms;
}

void BtUart_Poll(const WfPadState *state)
{
    WfFrame frame;
    uint32_t now = s_ms;

    PIN_USB_ACTIVE = g_hsusbd_Configured ? 1 : 0;

    if ((uint32_t)(now - s_last_ms) < 1u) {
        return;
    }
    s_last_ms = now;

    memset(&frame, 0, sizeof(frame));
    frame.seq = s_seq++;
    frame.dpad = state->dpad;
    frame.buttons = state->buttons;
    frame.lx = 0x7FFFu;
    frame.ly = 0x7FFFu;
    frame.rx = 0x7FFFu;
    frame.ry = 0x7FFFu;
    frame.flags = WF_LINK_AUTO;
    wf_frame_seal(&frame);
    uart1_write((const uint8_t *)&frame, WF_FRAME_SIZE);
}
