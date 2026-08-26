#ifndef GP2040_WF_PROTOCOL_H_
#define GP2040_WF_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WF_MAGIC0           0x57u /* 'W' */
#define WF_MAGIC1           0x46u /* 'F' */
#define WF_FRAME_SIZE       20u
#define WF_UART_BAUD        1000000u
#define WF_RADIO_CHANNEL    80u   /* nRF proprietary: 2480 MHz */
#define WF_ESPNOW_CHANNEL   13u   /* ESP-NOW 2.4G dongle, Wi-Fi ch 13, not AP */
#define WF_PIPE_ADDR        { 0xE7u, 0xE7u, 0xE7u, 0xE7u, 0x57u }

#define WF_LINK_AUTO        0u
#define WF_LINK_24G         1u
#define WF_LINK_BLE         2u
#define WF_LINK_MASK        0x0Fu

#define WF_MODE_SHIFT       4u
#define WF_MODE_MASK        0xF0u
#define WF_MODE_PC          0u /* generic HID, PC Bluetooth */
#define WF_MODE_SWITCH      1u /* Switch Pro, no crypto */
#define WF_MODE_PS          2u /* wired USB auth only */
#define WF_MODE_XBOX        3u /* wired USB auth only */

static inline uint8_t wf_flags_pack(uint8_t link, uint8_t mode)
{
    return (uint8_t)((link & WF_LINK_MASK) | ((mode << WF_MODE_SHIFT) & WF_MODE_MASK));
}

static inline uint8_t wf_flags_link(uint8_t flags)
{
    return (uint8_t)(flags & WF_LINK_MASK);
}

static inline uint8_t wf_flags_mode(uint8_t flags)
{
    return (uint8_t)((flags & WF_MODE_MASK) >> WF_MODE_SHIFT);
}

typedef struct __attribute__((packed)) {
    uint8_t  magic0;
    uint8_t  magic1;
    uint8_t  seq;
    uint8_t  dpad;
    uint32_t buttons;
    uint16_t lx;
    uint16_t ly;
    uint16_t rx;
    uint16_t ry;
    uint8_t  lt;
    uint8_t  rt;
    uint8_t  flags;
    uint8_t  crc;
} WfFrame;

static inline uint8_t wf_crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

static inline void wf_frame_seal(WfFrame *frame)
{
    frame->magic0 = WF_MAGIC0;
    frame->magic1 = WF_MAGIC1;
    frame->crc = wf_crc8((const uint8_t *)frame, WF_FRAME_SIZE - 1);
}

static inline int wf_frame_valid(const WfFrame *frame)
{
    if (frame->magic0 != WF_MAGIC0 || frame->magic1 != WF_MAGIC1) {
        return 0;
    }
    return wf_crc8((const uint8_t *)frame, WF_FRAME_SIZE - 1) == frame->crc;
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
static_assert(sizeof(WfFrame) == WF_FRAME_SIZE, "WfFrame must be 20 bytes");
#else
_Static_assert(sizeof(WfFrame) == WF_FRAME_SIZE, "WfFrame must be 20 bytes");
#endif

#endif
