#ifndef GP2040_WF_SWITCH_PRO_H_
#define GP2040_WF_SWITCH_PRO_H_

#include "protocol.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WF_SWITCH_PRO_REPORT_SIZE 64
#define WF_SWITCH_USB 0
#define WF_SWITCH_BT  1

void wf_switch_pro_init(int transport);
void wf_switch_pro_apply_frame(const WfFrame *frame);
void wf_switch_pro_on_output(uint8_t report_id, const uint8_t *data, uint16_t len);
int  wf_switch_pro_next_report(uint8_t out[WF_SWITCH_PRO_REPORT_SIZE]);
int  wf_switch_pro_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif
