#ifndef STATUS_BAR_H_
#define STATUS_BAR_H_

#include <string>
#include "helper.h"

#ifndef BATTERY_LOW_PIN
#define BATTERY_LOW_PIN -1
#endif

#ifndef BATTERY_LED_INDEX
#define BATTERY_LED_INDEX -1
#endif

#ifndef BATTERY_LED_RED_PIN
#define BATTERY_LED_RED_PIN -1
#endif

#ifndef BATTERY_LED_GREEN_PIN
#define BATTERY_LED_GREEN_PIN -1
#endif

/* 'L' wired USB, 'B' Bluetooth (Jerry UART), 'G' 2.4G (nRF). */
char statusLinkLetter();

/* USB plugged in is always OK. Otherwise a voltage-detector GPIO (active-low). */
bool batteryVoltageLow();

void batteryLedGpioSetup();
void batteryLedGpioProcess();

/* Right-align the link letter on a 21-char OLED header. */
void appendStatusLinkBattery(std::string& bar, int width = 21);

#endif
