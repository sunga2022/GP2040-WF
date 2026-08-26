#include "radio_link.h"
#include "../protocol.h"
#include <string.h>

#if defined(NRF52) || defined(NRF52840_XXAA) || defined(ARDUINO_ARCH_NRF52)

#include "nrf.h"

static uint8_t packet[32];
static uint8_t tx_mode;

static void hfclk_start(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    }
}

void wf_radio_init(int is_tx)
{
    const uint8_t addr[] = WF_PIPE_ADDR;
    tx_mode = is_tx ? 1 : 0;
    hfclk_start();

    NRF_RADIO->POWER = 1;
    NRF_RADIO->MODE = RADIO_MODE_MODE_Nrf_1Mbit;
    NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm;
    NRF_RADIO->FREQUENCY = WF_RADIO_CHANNEL;
    NRF_RADIO->BASE0 = (uint32_t)addr[0] | ((uint32_t)addr[1] << 8) | ((uint32_t)addr[2] << 16) | ((uint32_t)addr[3] << 24);
    NRF_RADIO->PREFIX0 = addr[4];
    NRF_RADIO->TXADDRESS = 0;
    NRF_RADIO->RXADDRESSES = 1;
    NRF_RADIO->PCNF0 = (8 << RADIO_PCNF0_LFLEN_Pos);
    NRF_RADIO->PCNF1 =
        (32 << RADIO_PCNF1_MAXLEN_Pos) |
        (32 << RADIO_PCNF1_STATLEN_Pos) |
        (4 << RADIO_PCNF1_BALEN_Pos) |
        (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos);
    NRF_RADIO->CRCCNF = RADIO_CRCCNF_LEN_Two;
    NRF_RADIO->CRCPOLY = 0x11021;
    NRF_RADIO->CRCINIT = 0xFFFF;
    NRF_RADIO->PACKETPTR = (uint32_t)packet;
    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk;

    if (!tx_mode) {
        NRF_RADIO->TASKS_RXEN = 1;
    }
}

int wf_radio_send(const uint8_t *data, size_t len)
{
    if (!tx_mode || data == 0) {
        return 0;
    }
    if (len > 32) {
        len = 32;
    }
    memset(packet, 0, sizeof(packet));
    memcpy(packet, data, len);
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {
    }
    return 1;
}

int wf_radio_recv(uint8_t *data, size_t len)
{
    if (tx_mode || data == 0) {
        return 0;
    }
    if (NRF_RADIO->EVENTS_DISABLED == 0) {
        return 0;
    }
    NRF_RADIO->EVENTS_DISABLED = 0;
    if (NRF_RADIO->CRCSTATUS == 1) {
        if (len > 32) {
            len = 32;
        }
        memcpy(data, packet, len);
        NRF_RADIO->TASKS_RXEN = 1;
        return 1;
    }
    NRF_RADIO->TASKS_RXEN = 1;
    return 0;
}

#else

void wf_radio_init(int is_tx) { (void)is_tx; }
int wf_radio_send(const uint8_t *data, size_t len) { (void)data; (void)len; return 0; }
int wf_radio_recv(uint8_t *data, size_t len) { (void)data; (void)len; return 0; }

#endif
