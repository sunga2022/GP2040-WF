#ifndef RF_LINK_H
#define RF_LINK_H

#include "protocol.h"

#ifndef WF_ENABLE_RF
#define WF_ENABLE_RF 0
#endif

void rf_link_init(int is_dongle);
int rf_link_send(const WfFrame *frame);
int rf_link_recv(WfFrame *frame);

#endif
