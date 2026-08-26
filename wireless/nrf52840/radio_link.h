/*
 * GP2040-WF nRF52840 2.4 GHz link
 * Nordic proprietary 1 Mbps, 32-byte payload, no ACK (1 kHz fire-and-forget).
 */
#ifndef WF_RADIO_LINK_H_
#define WF_RADIO_LINK_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void wf_radio_init(int is_tx);
int  wf_radio_send(const uint8_t *data, size_t len);
int  wf_radio_recv(uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
