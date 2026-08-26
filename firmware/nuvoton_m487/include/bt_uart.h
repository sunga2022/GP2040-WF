#ifndef BT_UART_H
#define BT_UART_H

#include "hid_gamepad.h"

void BtUart_Init(void);
void BtUart_Poll(const WfPadState *state);

#endif
