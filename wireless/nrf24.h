#ifndef GP2040_WF_NRF24_H_
#define GP2040_WF_NRF24_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int ce;
    int csn;
    int sck;
    int mosi;
    int miso; /* -1 if TX-only */
} WfNrf24Pins;

void wf_nrf24_init(const WfNrf24Pins *pins, int is_tx);
int  wf_nrf24_send(const uint8_t *data, size_t len);
int  wf_nrf24_recv(uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
