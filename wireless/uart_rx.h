#ifndef GP2040_WF_UART_RX_H_
#define GP2040_WF_UART_RX_H_

#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t buf[WF_FRAME_SIZE];
    uint8_t n;
} WfUartRx;

static inline void wf_uart_rx_reset(WfUartRx *rx)
{
    rx->n = 0;
}

/*
 * Feed one UART byte. Returns 1 and writes *out when a sealed WfFrame is complete.
 */
static inline int wf_uart_rx_byte(WfUartRx *rx, uint8_t byte, WfFrame *out)
{
    if (rx->n == 0u && byte != WF_MAGIC0) {
        return 0;
    }
    if (rx->n == 1u && byte != WF_MAGIC1) {
        rx->n = 0;
        return 0;
    }
    rx->buf[rx->n++] = byte;
    if (rx->n < WF_FRAME_SIZE) {
        return 0;
    }
    rx->n = 0;
    memcpy(out, rx->buf, WF_FRAME_SIZE);
    return wf_frame_valid(out);
}

#ifdef __cplusplus
}
#endif

#endif
