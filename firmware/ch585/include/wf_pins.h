#ifndef WF_PINS_H
#define WF_PINS_H

#include "CH58x_common.h"

/*
 * CH585M-EVT / CH585 QFN48 按键脚（低电平有效，内部上拉）。
 *
 * 避开：
 *   USB HS PHY（独立 D+/D-，不是 GPIO）
 *   PA14/PA15 默认 UART0 调试
 *   PA8/PA9 若当 UART1 用
 *
 * 杜邦线接到评估板排针即可。LED 接 PA0（评估板 LED 要自己跳线）。
 */

#define PIN_PORT_DPAD     GPIOB
#define PIN_UP            GPIO_Pin_0
#define PIN_DOWN          GPIO_Pin_1
#define PIN_LEFT          GPIO_Pin_2
#define PIN_RIGHT         GPIO_Pin_3

#define PIN_B1            GPIO_Pin_4   /* A / Cross / K1 */
#define PIN_B2            GPIO_Pin_5   /* B / Circle / K2 */
#define PIN_B3            GPIO_Pin_6   /* X / Square / P1 */
#define PIN_B4            GPIO_Pin_7   /* Y / Triangle / P2 */
#define PIN_L1            GPIO_Pin_8
#define PIN_R1            GPIO_Pin_9
#define PIN_L2            GPIO_Pin_10
#define PIN_R2            GPIO_Pin_11
#define PIN_S1            GPIO_Pin_12  /* Select */
#define PIN_S2            GPIO_Pin_13  /* Start */
#define PIN_L3            GPIO_Pin_14
#define PIN_R3            GPIO_Pin_15

#define PIN_A1            GPIO_Pin_8   /* PA8  Home / PS */
#define PIN_A2            GPIO_Pin_9   /* PA9  Capture */
#define PIN_LED           GPIO_Pin_0   /* PA0 */
#define PIN_BIND          GPIO_Pin_10  /* PA10 接收器配对键 */

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

static inline int pin_pressed_b(uint32_t pin)
{
    return GPIOB_ReadPortPin(pin) == 0;
}

static inline int pin_pressed_a(uint32_t pin)
{
    return GPIOA_ReadPortPin(pin) == 0;
}

#endif
