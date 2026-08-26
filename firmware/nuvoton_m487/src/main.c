#include "NuMicro.h"
#include "hid_usb.h"
#include "hid_gamepad.h"

static void SYS_Init(void)
{
    uint32_t volatile i;

    SYS_UnlockReg();

    /* HXT on PF.2 / PF.3 (NuMaker 12 MHz crystal). */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(1));
    CLK_SetCoreClock(FREQ_192MHZ);
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;

    /* High-speed USB device PHY (CON1 on NuMaker-PFM-M487). */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk))
                  | SYS_USBPHY_HSUSBEN_Msk;
    for (i = 0; i < 0x1000; i++) {
    }
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    CLK_EnableModuleClock(HSUSBD_MODULE);
}

int main(void)
{
    SYS_Init();
    Gamepad_GpioInit();
    HID_DescriptorsInit();

    HSUSBD_Open(&gsHSInfo, HID_ClassRequest, NULL);
    HID_Init();
    NVIC_EnableIRQ(USBD20_IRQn);
    HSUSBD_Start();

    while (1) {
        HID_Process();
    }
}
