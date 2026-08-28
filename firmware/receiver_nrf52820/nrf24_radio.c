#include "nrf24_radio.h"
#include "protocol.h"
#include "nrf.h"
#include <string.h>

static uint8_t rxpkt[32] __attribute__((aligned(4)));

void nrf24_radio_init(void)
{
    static const uint8_t pipe[5] = WF_PIPE_ADDR;

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    }

    NRF_RADIO->POWER = 1;
    NRF_RADIO->MODE = RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos;
    NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos;
    NRF_RADIO->FREQUENCY = WF_RADIO_CHANNEL;
    /* ShockBurst: no S0/length/S1, static 20-byte payload, 5-byte address. */
    NRF_RADIO->PCNF0 = 0;
    NRF_RADIO->PCNF1 =
        (WF_FRAME_SIZE << RADIO_PCNF1_MAXLEN_Pos) |
        (WF_FRAME_SIZE << RADIO_PCNF1_STATLEN_Pos) |
        (4 << RADIO_PCNF1_BALEN_Pos) |
        (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos);
    NRF_RADIO->CRCCNF =
        (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos) |
        (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);
    NRF_RADIO->CRCPOLY = 0x11021;
    NRF_RADIO->CRCINIT = 0xFFFF;
    /* pipe SPI-order {E7,E7,E7,E7,57}: first on-air byte is PREFIX. */
    NRF_RADIO->PREFIX0 = pipe[0];
    NRF_RADIO->BASE0 = ((uint32_t)pipe[1] << 24) | ((uint32_t)pipe[2] << 16) |
                       ((uint32_t)pipe[3] << 8) | (uint32_t)pipe[4];
    NRF_RADIO->TXADDRESS = 0;
    NRF_RADIO->RXADDRESSES = 1;
    NRF_RADIO->PACKETPTR = (uint32_t)rxpkt;
    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_START_Msk;
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_RXEN = 1;
}

int nrf24_radio_recv(uint8_t *payload, size_t maxlen)
{
    if (NRF_RADIO->EVENTS_END == 0) {
        return 0;
    }
    NRF_RADIO->EVENTS_END = 0;
    if (NRF_RADIO->CRCSTATUS == 0) {
        return 0;
    }
    size_t n = maxlen < WF_FRAME_SIZE ? maxlen : WF_FRAME_SIZE;
    memcpy(payload, rxpkt, n);
    return (int)n;
}
