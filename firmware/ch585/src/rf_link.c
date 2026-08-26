#include "rf_link.h"
#include <string.h>

#if WF_ENABLE_RF

#include "wchrf.h"

#ifndef WF_RF_FREQUENCY
#define WF_RF_FREQUENCY 16
#endif

static __attribute__((aligned(4))) uint8_t s_tx[64];
static __attribute__((aligned(4))) uint8_t s_rx[264];
static rfipTx_t s_txp;
static rfipRx_t s_rxp;
static volatile uint8_t s_have;
static WfFrame s_last;

void rf_link_init(int is_dongle)
{
    memset(&s_txp, 0, sizeof(s_txp));
    memset(&s_rxp, 0, sizeof(s_rxp));
    s_txp.frequency = WF_RF_FREQUENCY;
    s_txp.txDMA = (uint32_t)s_tx;
    s_txp.sendCount = 1;
    s_rxp.frequency = WF_RF_FREQUENCY;
    s_rxp.rxDMA = (uint32_t)s_rx;
    s_rxp.rxMaxLen = 64;
    s_rxp.timeOut = 0;
    if (is_dongle) {
        RFIP_SetRx(&s_rxp);
    }
}

int rf_link_send(const WfFrame *frame)
{
    s_tx[0] = 0x55;
    s_tx[1] = (uint8_t)sizeof(*frame);
    memcpy(&s_tx[2], frame, sizeof(*frame));
    RFIP_SetTxStart();
    RFIP_SetTxParm(&s_txp);
    return 0;
}

int rf_link_recv(WfFrame *frame)
{
    const uint8_t *pkt = (const uint8_t *)s_rxp.rxDMA;
    WfFrame tmp;

    if (pkt == 0 || pkt[1] < sizeof(WfFrame)) {
        return 0;
    }
    memcpy(&tmp, &pkt[2], sizeof(tmp));
    if (!wf_frame_valid(&tmp)) {
        return 0;
    }
    if (s_have) {
        *frame = s_last;
        s_have = 0;
        return 1;
    }
    *frame = tmp;
    return 1;
}

#else

void rf_link_init(int is_dongle)
{
    (void)is_dongle;
}

int rf_link_send(const WfFrame *frame)
{
    (void)frame;
    return -1;
}

int rf_link_recv(WfFrame *frame)
{
    (void)frame;
    return 0;
}

#endif
