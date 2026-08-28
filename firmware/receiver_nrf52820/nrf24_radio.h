#ifndef GP2040_WF_NRF24_RADIO_H_
#define GP2040_WF_NRF24_RADIO_H_

#include <stdint.h>
#include <stddef.h>

void nrf24_radio_init(void);
int  nrf24_radio_recv(uint8_t *payload, size_t maxlen);

#endif
