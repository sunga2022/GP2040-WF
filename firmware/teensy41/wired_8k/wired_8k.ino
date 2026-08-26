/*
 * GP2040-WF 有线 8 kHz 原型 —— Teensy 4.1（NXP i.MX RT1062，USB High Speed）
 *
 * 这颗芯片才有 USB 2.0 HS。bInterval=1 在 HS 上是 125 μs = 8000 Hz。
 * ESP32-S3 / nRF52840 / RP2040 都是 Full Speed，做不到 8k。
 *
 * Arduino IDE:
 *   开发板: Teensy 4.1
 *   USB Type: Keyboard + Mouse + Joystick
 *
 * 还要把 Teensyduino 核心里的轮询改成 1:
 *   hardware/teensy/avr/cores/teensy4/usb_desc.h
 *   找到 JOYSTICK_INTERVAL，改成 1
 *   （默认往往是 1ms 量级，不改就仍是 1k 而不是 8k）
 *
 * 没有射频，不要拿这块板做蓝牙/2.4G。
 */

#include <Arduino.h>

static const int kPins[] = {
    2, 3, 4, 5,     // UP DOWN LEFT RIGHT
    6, 7, 8, 9,     // B1 B2 B3 B4
    10, 11, 12, 14, // L1 R1 L2 R2
    15, 16, 17, 18, // S1 S2 L3 R3
    19, 20          // A1 A2
};
static const int kCount = sizeof(kPins) / sizeof(kPins[0]);

void setup()
{
    for (int i = 0; i < kCount; i++) {
        pinMode(kPins[i], INPUT_PULLUP);
    }
    Joystick.useManualSend(true);
}

void loop()
{
    for (int i = 0; i < kCount; i++) {
        Joystick.button(i + 1, digitalRead(kPins[i]) == LOW);
    }
    const bool up = digitalRead(2) == LOW;
    const bool down = digitalRead(3) == LOW;
    const bool left = digitalRead(4) == LOW;
    const bool right = digitalRead(5) == LOW;
    int hat = -1;
    if (up && right) hat = 45;
    else if (right && down) hat = 135;
    else if (down && left) hat = 225;
    else if (left && up) hat = 315;
    else if (up) hat = 0;
    else if (right) hat = 90;
    else if (down) hat = 180;
    else if (left) hat = 270;
    Joystick.hat(hat);
    Joystick.send_now();
}
