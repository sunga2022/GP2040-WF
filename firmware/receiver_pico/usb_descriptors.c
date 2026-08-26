#include "tusb.h"
#include "switch_pro_desc.h"
#include <string.h>

uint8_t const * tud_descriptor_device_cb(void)
{
    return wf_switch_pro_device_descriptor;
}

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return wf_switch_pro_report_descriptor;
}

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return wf_switch_pro_configuration_descriptor;
}

static char const *string_desc_arr[] = {
    (const char[]){ 0x09, 0x04 },
    "Open Stick Community",
    "GP2040-CE (Pro Controller)",
    "000000000001",
};

static uint16_t _desc_str[32];

uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint8_t chr_count;
    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) {
            return NULL;
        }
        const char *str = string_desc_arr[index];
        chr_count = (uint8_t)strlen(str);
        if (chr_count > 31) {
            chr_count = 31;
        }
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }
    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
    return _desc_str;
}
