#ifndef HID_USB_H
#define HID_USB_H

#include <stdint.h>
#include "hid_gamepad.h"

/* Nuvoton VID. PID is unique to this gamepad firmware (mouse sample is 0x8249). */
#define USBD_VID                0x0416
#define USBD_PID                0xB204

#define GET_REPORT              0x01
#define GET_IDLE                0x02
#define GET_PROTOCOL            0x03
#define SET_REPORT              0x09
#define SET_IDLE                0x0A
#define SET_PROTOCOL            0x0B

#define HID_NONE                0x00
#define HID_BOOT_PROTOCOL       0x00
#define HID_REPORT_PROTOCOL     0x01

#define CEP_MAX_PKT_SIZE        64
#define CEP_OTHER_MAX_PKT_SIZE  64
#define EPA_MAX_PKT_SIZE        8
#define EPA_OTHER_MAX_PKT_SIZE  8

#define CEP_BUF_BASE    0
#define CEP_BUF_LEN     CEP_MAX_PKT_SIZE
#define EPA_BUF_BASE    (CEP_BUF_BASE + CEP_BUF_LEN)
#define EPA_BUF_LEN     EPA_MAX_PKT_SIZE

#define INT_IN_EP_NUM   0x01

/*
 * USB HS bInterval=1 → 2^(1-1)*125 µs = 125 µs = 8000 Hz.
 * Official Nuvoton 8K HID mouse samples use 1. Stock BSP mouse uses 10 (125 Hz).
 * USB FS other-speed descriptor also uses 1 → 1 ms = 1000 Hz if the host falls back.
 */
#define HID_DEFAULT_INT_IN_INTERVAL     1
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  50  /* unit 2 mA → 100 mA */

#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE+LEN_HID+LEN_ENDPOINT)

#define HID_REPORT_DESC_LEN             71

void HID_DescriptorsInit(void);
void HID_InitForHighSpeed(void);
void HID_InitForFullSpeed(void);
void HID_Init(void);
void HID_ClassRequest(void);
void EPA_Handler(void);
void HID_Process(void);
void HID_SendIfReady(const WfPadState *state);

#endif
