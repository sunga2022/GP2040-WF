#pragma once

// ESP32-S3-DevKitC-1 按键脚。避开 USB D+/D-（19/20）和启动脚（0/3/45/46）。
// 全部低电平有效，内部上拉。

#define PIN_UP      4
#define PIN_DOWN    5
#define PIN_LEFT    6
#define PIN_RIGHT   7
#define PIN_B1      8   // A / Cross / K1
#define PIN_B2      9   // B / Circle / K2
#define PIN_B3      10  // X / Square / P1
#define PIN_B4      11  // Y / Triangle / P2
#define PIN_L1      12
#define PIN_R1      13
#define PIN_L2      14
#define PIN_R2      17
#define PIN_S1      18  // Select / Back
#define PIN_S2      21  // Start
#define PIN_L3      38
#define PIN_R3      39
#define PIN_A1      40  // Guide / Home / PS
#define PIN_A2      41  // Capture
#define PIN_LED     48

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
