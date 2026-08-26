#ifndef WF_PINS_H
#define WF_PINS_H

#include "NuMicro.h"

/*
 * NuMaker-PFM-M487 v3.0 Arduino headers (active-low, internal pull-up).
 *
 * Plug the fightstick into these pins. USB HS is CON1, not the Nu-Link ICE
 * micro-USB. RGB LED is PH.0 (active-low). User button SW2 is PF.11.
 *
 * Avoided: D0/D1 (PB.2/PB.3 UART0), PB.10/PB.11 (HSUSB VBUS EN/ST),
 * PF.2/PF.3 (12 MHz HXT).
 */

#define PIN_UP_PORT     PC
#define PIN_UP_BIT      BIT9    /* D2  PC.9  */
#define PIN_DOWN_PORT   PC
#define PIN_DOWN_BIT    BIT10   /* D3  PC.10 */
#define PIN_LEFT_PORT   PC
#define PIN_LEFT_BIT    BIT11   /* D4  PC.11 */
#define PIN_RIGHT_PORT  PC
#define PIN_RIGHT_BIT   BIT12   /* D5  PC.12 */

#define PIN_B1_PORT     PE
#define PIN_B1_BIT      BIT4    /* D6  PE.4  A / Cross / K1 */
#define PIN_B2_PORT     PE
#define PIN_B2_BIT      BIT5    /* D7  PE.5  B / Circle / K2 */
#define PIN_B3_PORT     PA
#define PIN_B3_BIT      BIT5    /* D8  PA.5  X / Square / P1 */
#define PIN_B4_PORT     PA
#define PIN_B4_BIT      BIT4    /* D9  PA.4  Y / Triangle / P2 */
#define PIN_L1_PORT     PA
#define PIN_L1_BIT      BIT3    /* D10 PA.3 */
#define PIN_R1_PORT     PA
#define PIN_R1_BIT      BIT0    /* D11 PA.0 */
#define PIN_L2_PORT     PA
#define PIN_L2_BIT      BIT1    /* D12 PA.1 */
#define PIN_R2_PORT     PA
#define PIN_R2_BIT      BIT2    /* D13 PA.2 */
#define PIN_S1_PORT     PB
#define PIN_S1_BIT      BIT6    /* A0  PB.6  Select */
#define PIN_S2_PORT     PB
#define PIN_S2_BIT      BIT7    /* A1  PB.7  Start */
#define PIN_L3_PORT     PB
#define PIN_L3_BIT      BIT8    /* A2  PB.8 */
#define PIN_R3_PORT     PB
#define PIN_R3_BIT      BIT9    /* A3  PB.9 */
#define PIN_A1_PORT     PB
#define PIN_A1_BIT      BIT0    /* A4  PB.0  Home / PS */
#define PIN_A2_PORT     PB
#define PIN_A2_BIT      BIT1    /* A5  PB.1  Capture */

#define PIN_LED         PH0     /* RGB red, active-low */
#define PIN_USER_PORT   PF
#define PIN_USER_BIT    BIT11   /* SW2 */

#define BTN_B1   (1u << 0)
#define BTN_B2   (1u << 1)
#define BTN_B3   (1u << 2)
#define BTN_B4   (1u << 3)
#define BTN_L1   (1u << 4)
#define BTN_R1   (1u << 5)
#define BTN_L2   (1u << 6)
#define BTN_R2   (1u << 7)
#define BTN_S1   (1u << 8)
#define BTN_S2   (1u << 9)
#define BTN_L3   (1u << 10)
#define BTN_R3   (1u << 11)
#define BTN_A1   (1u << 12)
#define BTN_A2   (1u << 13)

#define DPAD_UP    (1u << 0)
#define DPAD_DOWN  (1u << 1)
#define DPAD_LEFT  (1u << 2)
#define DPAD_RIGHT (1u << 3)

static inline int pin_pressed(GPIO_T *port, uint32_t bit)
{
    return (port->PIN & bit) == 0u;
}

#endif
