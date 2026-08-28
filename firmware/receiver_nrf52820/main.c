#include "nrf.h"
#include "nrfx_power.h"
#include "tusb.h"
#include "switch_pro.h"
#include "protocol.h"
#include "nrf24_radio.h"
#include <string.h>

static volatile uint32_t g_ms;
static uint32_t last_ms;
static int have_packet;

void SysTick_Handler(void)
{
    g_ms++;
}

void USBD_IRQHandler(void)
{
    tud_int_handler(0);
}

extern void tusb_hal_nrf_power_event(uint32_t event);

static void power_event_handler(nrfx_power_usb_evt_t event)
{
    tusb_hal_nrf_power_event((uint32_t)event);
}

static void usb_init(void)
{
    NVIC_SetPriority(USBD_IRQn, 2);
    const nrfx_power_config_t pwr_cfg = { 0 };
    nrfx_power_init(&pwr_cfg);
    const nrfx_power_usbevt_config_t config = { .handler = power_event_handler };
    nrfx_power_usbevt_init(&config);
    nrfx_power_usbevt_enable();

    uint32_t usb_reg = NRF_POWER->USBREGSTATUS;
    if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
        tusb_hal_nrf_power_event(0);
    }
    if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
        tusb_hal_nrf_power_event(2);
    }
}

int main(void)
{
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    SysTick_Config(SystemCoreClock / 1000);

    wf_switch_pro_init(WF_SWITCH_USB);
    nrf24_radio_init();
    usb_init();
    tusb_init();

    last_ms = g_ms;
    uint8_t rxbuf[WF_FRAME_SIZE];
    while (1) {
        tud_task();

        if (nrf24_radio_recv(rxbuf, sizeof(rxbuf))) {
            WfFrame frame;
            memcpy(&frame, rxbuf, sizeof(frame));
            if (wf_frame_valid(&frame)) {
                wf_switch_pro_apply_frame(&frame);
                last_ms = g_ms;
                have_packet = 1;
            }
        }

        if (have_packet && (g_ms - last_ms) > 50) {
            WfFrame idle;
            memset(&idle, 0, sizeof(idle));
            idle.lx = idle.ly = idle.rx = idle.ry = 0x7FFF;
            wf_switch_pro_apply_frame(&idle);
            have_packet = 0;
        }

        if (tud_hid_ready()) {
            uint8_t hid[WF_SWITCH_PRO_REPORT_SIZE];
            (void)wf_switch_pro_next_report(hid);
            tud_hid_report(0, hid, WF_SWITCH_PRO_REPORT_SIZE);
        }
    }
}
