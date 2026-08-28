#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"
#include "switch_pro_desc.h"

#define FUSB_BUFFERS_NUMBER   4
#define FUSB_EP1_MODE         USBFS_EP_MODE_BDIR
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_HID_INTERFACES   1
#define FUSB_CURSED_TURBO_DMA 0
#define FUSB_HID_USER_REPORTS 1
#define FUSB_IO_PROFILE       0
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_USER_HANDLERS    1
#define FUSB_USE_DMA7_COPY    0
#define FUSB_VDD_5V           0
#define FUSB_FROM_RAM         0

#include "usb_defines.h"

#define FUSB_USB_VID 0x057E
#define FUSB_USB_PID 0x2009
#define FUSB_USB_REV 0x0210
#define FUSB_STR_MANUFACTURER u"Nintendo Co., Ltd."
#define FUSB_STR_PRODUCT      u"Pro Controller"
#define FUSB_STR_SERIAL       u"000000000001"

static const uint8_t device_descriptor[] = {
    18, 1,
    0x00, 0x02,
    0x00, 0x00, 0x00,
    64,
    (uint8_t)FUSB_USB_VID, (uint8_t)(FUSB_USB_VID >> 8),
    (uint8_t)FUSB_USB_PID, (uint8_t)(FUSB_USB_PID >> 8),
    (uint8_t)FUSB_USB_REV, (uint8_t)(FUSB_USB_REV >> 8),
    1, 2, 3, 1,
};

static const uint8_t config_descriptor[] = {
    0x09, 0x02, 0x29, 0x00, 0x01, 0x01, 0x00, 0xA0, 0xFA,
    0x09, 0x04, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00,
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22,
    (uint8_t)sizeof(wf_switch_pro_report_descriptor),
    (uint8_t)(sizeof(wf_switch_pro_report_descriptor) >> 8),
    /* USB FS interrupt bInterval=1 → 1 ms → 1 kHz polling */
    0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x01,
    0x07, 0x05, 0x01, 0x03, 0x40, 0x00, 0x01,
};

struct usb_string_descriptor_struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wString[];
};

const static struct usb_string_descriptor_struct string0 __attribute__((section(".rodata"))) = {
    4, 3, { 0x0409 }
};
const static struct usb_string_descriptor_struct string1 __attribute__((section(".rodata"))) = {
    sizeof(FUSB_STR_MANUFACTURER), 3, FUSB_STR_MANUFACTURER
};
const static struct usb_string_descriptor_struct string2 __attribute__((section(".rodata"))) = {
    sizeof(FUSB_STR_PRODUCT), 3, FUSB_STR_PRODUCT
};
const static struct usb_string_descriptor_struct string3 __attribute__((section(".rodata"))) = {
    sizeof(FUSB_STR_SERIAL), 3, FUSB_STR_SERIAL
};

const static struct descriptor_list_struct {
    uint32_t lIndexValue;
    const uint8_t *addr;
    uint8_t length;
} descriptor_list[] = {
    { 0x00000100, device_descriptor, sizeof(device_descriptor) },
    { 0x00000200, config_descriptor, sizeof(config_descriptor) },
    { 0x00002200, wf_switch_pro_report_descriptor, sizeof(wf_switch_pro_report_descriptor) },
    { 0x00002100, config_descriptor + 18, 9 },
    { 0x00000300, (const uint8_t *)&string0, 4 },
    { 0x04090301, (const uint8_t *)&string1, string1.bLength },
    { 0x04090302, (const uint8_t *)&string2, string2.bLength },
    { 0x04090303, (const uint8_t *)&string3, string3.bLength },
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list)) / (sizeof(struct descriptor_list_struct)))

#endif
