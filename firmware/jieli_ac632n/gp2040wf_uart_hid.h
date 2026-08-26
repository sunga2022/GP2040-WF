#ifndef GP2040WF_UART_HID_H_
#define GP2040WF_UART_HID_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void wf_jieli_reset(void);
void wf_jieli_on_byte(uint8_t b);
void wf_jieli_on_hid_output(uint8_t report_id, const uint8_t *data, uint16_t len);
void wf_jieli_poll(void);

#ifdef __cplusplus
}
#endif

#endif
