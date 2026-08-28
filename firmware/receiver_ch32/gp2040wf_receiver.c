#include "ch32fun.h"
#include "fsusb.h"
#include "nrf24.h"
#include "switch_pro.h"
#include "protocol.h"
#include <string.h>

#ifndef CE_PIN
#define CE_PIN  PA0
#endif
#ifndef CSN_PIN
#define CSN_PIN PA1
#endif
#ifndef SCK_PIN
#define SCK_PIN PA2
#endif
#ifndef MOSI_PIN
#define MOSI_PIN PA3
#endif
#ifndef MISO_PIN
#define MISO_PIN PA4
#endif

#define LINK_TIMEOUT_TICKS (FUNCONF_SYSTEM_CORE_CLOCK / 20u)

static uint8_t rxbuf[32];
static uint32_t lastPacket;
static int havePacket;

int HandleHidUserSetReportSetup(struct _USBState *ctx, tusb_control_request_t *req)
{
    (void)ctx;
    (void)req;
    return 0;
}

int HandleHidUserGetReportSetup(struct _USBState *ctx, tusb_control_request_t *req)
{
    (void)ctx;
    (void)req;
    return 0;
}

void HandleHidUserReportDataOut(struct _USBState *ctx, uint8_t *data, int len)
{
    (void)ctx;
    if (data && len > 0) {
        wf_switch_pro_on_output(data[0], data, (uint16_t)len);
    }
}

int HandleHidUserReportDataIn(struct _USBState *ctx, uint8_t *data, int len)
{
    (void)ctx;
    (void)data;
    return len;
}

void HandleHidUserReportOutComplete(struct _USBState *ctx)
{
    (void)ctx;
}

int HandleInRequest(struct _USBState *ctx, int endp, uint8_t *data, int len)
{
    (void)ctx;
    (void)endp;
    (void)data;
    return len;
}

void HandleDataOut(struct _USBState *ctx, int endp, uint8_t *data, int len)
{
    (void)ctx;
    if (endp == 1 && data && len > 0) {
        wf_switch_pro_on_output(data[0], data, (uint16_t)len);
    }
}

int HandleSetupCustom(struct _USBState *ctx, int setup_code)
{
    (void)ctx;
    (void)setup_code;
    return 0;
}

int main(void)
{
    SystemInit();
    funGpioInitAll();

    wf_switch_pro_init(WF_SWITCH_USB);
    WfNrf24Pins pins = { CE_PIN, CSN_PIN, SCK_PIN, MOSI_PIN, MISO_PIN };
    wf_nrf24_init(&pins, 0);
    USBFSSetup();

    lastPacket = SysTick->CNT;
    while (1) {
        if (wf_nrf24_recv(rxbuf, WF_FRAME_SIZE)) {
            WfFrame frame;
            memcpy(&frame, rxbuf, sizeof(frame));
            if (wf_frame_valid(&frame)) {
                wf_switch_pro_apply_frame(&frame);
                lastPacket = SysTick->CNT;
                havePacket = 1;
            }
        }

        if (havePacket && (uint32_t)(SysTick->CNT - lastPacket) > LINK_TIMEOUT_TICKS) {
            WfFrame idle;
            memset(&idle, 0, sizeof(idle));
            idle.lx = idle.ly = idle.rx = idle.ry = 0x7FFF;
            wf_switch_pro_apply_frame(&idle);
            havePacket = 0;
        }

        uint8_t *hid = USBFS_GetEPBufferIfAvailable(1);
        if (hid) {
            (void)wf_switch_pro_next_report(hid);
            USBFS_SendEndpoint(1, WF_SWITCH_PRO_REPORT_SIZE);
        }
    }
}
