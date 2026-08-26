#include "NuMicro.h"
#include "hid_usb.h"

uint8_t HID_GamepadReportDescriptor[] __attribute__((aligned(4))) = {
    0x05, 0x01,
    0x09, 0x05,
    0xA1, 0x01,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x10,
    0x05, 0x09,
    0x19, 0x01,
    0x29, 0x10,
    0x81, 0x02,
    0x05, 0x01,
    0x09, 0x39,
    0x15, 0x00,
    0x25, 0x07,
    0x35, 0x00,
    0x46, 0x3B, 0x01,
    0x65, 0x14,
    0x75, 0x04,
    0x95, 0x01,
    0x81, 0x42,
    0x65, 0x00,
    0x75, 0x04,
    0x95, 0x01,
    0x81, 0x01,
    0x09, 0x30,
    0x09, 0x31,
    0x09, 0x32,
    0x09, 0x35,
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x04,
    0x81, 0x02,
    0xC0
};

_Static_assert(sizeof(HID_GamepadReportDescriptor) == HID_REPORT_DESC_LEN,
               "HID report descriptor length");
_Static_assert(HID_DEFAULT_INT_IN_INTERVAL == 1, "HS bInterval=1 is 8 kHz");

uint8_t gu8DeviceDescriptor[] __attribute__((aligned(4))) = {
    LEN_DEVICE,
    DESC_DEVICE,
    0x00, 0x02,
    0x00,
    0x00,
    0x00,
    CEP_MAX_PKT_SIZE,
    USBD_VID & 0x00FF,
    (USBD_VID >> 8) & 0x00FF,
    USBD_PID & 0x00FF,
    (USBD_PID >> 8) & 0x00FF,
    0x00, 0x01,
    0x01,
    0x02,
    0x03,
    0x01
};

uint8_t gu8QualifierDescriptor[] __attribute__((aligned(4))) = {
    LEN_QUALIFIER,
    DESC_QUALIFIER,
    0x00, 0x02,
    0x00,
    0x00,
    0x00,
    CEP_OTHER_MAX_PKT_SIZE,
    0x01,
    0x00
};

static const uint8_t s_config_body_hs[] = {
    LEN_CONFIG,
    DESC_CONFIG,
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE >> 8) & 0x00FF,
    0x01,
    0x01,
    0x00,
    0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),
    USBD_MAX_POWER,

    LEN_INTERFACE,
    DESC_INTERFACE,
    0x00,
    0x00,
    0x01,
    0x03,
    0x00,
    HID_NONE,
    0x00,

    LEN_HID,
    DESC_HID,
    0x11, 0x01,
    0x00,
    0x01,
    DESC_HID_RPT,
    HID_REPORT_DESC_LEN & 0x00FF,
    (HID_REPORT_DESC_LEN >> 8) & 0x00FF,

    LEN_ENDPOINT,
    DESC_ENDPOINT,
    (INT_IN_EP_NUM | EP_INPUT),
    EP_INT,
    EPA_MAX_PKT_SIZE & 0x00FF,
    (EPA_MAX_PKT_SIZE >> 8) & 0x00FF,
    HID_DEFAULT_INT_IN_INTERVAL
};

uint8_t gu8ConfigDescriptor[sizeof(s_config_body_hs)] __attribute__((aligned(4)));
uint8_t gu8OtherConfigDescriptorHS[sizeof(s_config_body_hs)] __attribute__((aligned(4)));
uint8_t  gu8ConfigDescriptorFS[sizeof(s_config_body_hs)] __attribute__((aligned(4)));
uint8_t  gu8OtherConfigDescriptorFS[sizeof(s_config_body_hs)] __attribute__((aligned(4)));

static void fill_cfg(uint8_t *dst, uint8_t type, uint8_t mps_lo, uint8_t mps_hi)
{
    uint32_t i;
    for (i = 0; i < sizeof(s_config_body_hs); i++) {
        dst[i] = s_config_body_hs[i];
    }
    dst[1] = type;
    dst[sizeof(s_config_body_hs) - 3] = mps_lo;
    dst[sizeof(s_config_body_hs) - 2] = mps_hi;
}

uint8_t gu8StringLang[4] __attribute__((aligned(4))) = {
    4, DESC_STRING, 0x09, 0x04
};

uint8_t gu8VendorStringDesc[] __attribute__((aligned(4))) = {
    20, DESC_STRING,
    's', 0, 'u', 0, 'n', 0, 'g', 0, 'a', 0, '2', 0, '0', 0, '2', 0, '2', 0
};

uint8_t  gu8ProductStringDesc[] __attribute__((aligned(4))) = {
    20, DESC_STRING,
    'G', 0, 'P', 0, '2', 0, '0', 0, '4', 0, '0', 0, '-', 0, 'W', 0, 'F', 0
};

uint8_t gu8SerialStringDesc[] __attribute__((aligned(4))) = {
    10, DESC_STRING,
    'M', 0, '4', 0, '8', 0, '7', 0
};

uint8_t *gpu8UsbString[4] = {
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8SerialStringDesc,
};

uint8_t *gu8UsbHidReport[3] = {
    HID_GamepadReportDescriptor,
    NULL,
    NULL,
};

uint32_t gu32UsbHidReportLen[3] = {
    sizeof(HID_GamepadReportDescriptor),
    0,
    0,
};

uint32_t gu32ConfigHidDescIdx[3] = {
    (LEN_CONFIG + LEN_INTERFACE),
    0,
    0
};

S_HSUSBD_INFO_T gsHSInfo = {
    gu8DeviceDescriptor,
    gu8ConfigDescriptor,
    gpu8UsbString,
    gu8QualifierDescriptor,
    gu8ConfigDescriptorFS,
    gu8OtherConfigDescriptorHS,
    gu8OtherConfigDescriptorFS,
    gu8UsbHidReport,
    gu32UsbHidReportLen,
    gu32ConfigHidDescIdx,
};

void HID_DescriptorsInit(void)
{
    fill_cfg(gu8ConfigDescriptor, DESC_CONFIG,
             EPA_MAX_PKT_SIZE & 0xFF, (EPA_MAX_PKT_SIZE >> 8) & 0xFF);
    fill_cfg(gu8OtherConfigDescriptorHS, DESC_OTHERSPEED,
             EPA_OTHER_MAX_PKT_SIZE & 0xFF, (EPA_OTHER_MAX_PKT_SIZE >> 8) & 0xFF);
    fill_cfg(gu8ConfigDescriptorFS, DESC_CONFIG,
             EPA_OTHER_MAX_PKT_SIZE & 0xFF, (EPA_OTHER_MAX_PKT_SIZE >> 8) & 0xFF);
    fill_cfg(gu8OtherConfigDescriptorFS, DESC_OTHERSPEED,
             EPA_MAX_PKT_SIZE & 0xFF, (EPA_MAX_PKT_SIZE >> 8) & 0xFF);
}
