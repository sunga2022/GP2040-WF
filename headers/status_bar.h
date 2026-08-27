#ifndef STATUS_BAR_H_
#define STATUS_BAR_H_

#include <string>
#include "helper.h"

#ifndef BATTERY_LOW_PIN
#define BATTERY_LOW_PIN -1
#endif

#ifndef BATTERY_ADC_PIN
#define BATTERY_ADC_PIN -1
#endif

#ifndef BATTERY_ADC_SCALE
#define BATTERY_ADC_SCALE 2.0f
#endif

#ifndef BATTERY_EMPTY_MV
#define BATTERY_EMPTY_MV 3300
#endif

#ifndef BATTERY_FULL_MV
#define BATTERY_FULL_MV 4200
#endif

#ifndef BATTERY_LOW_MV
#define BATTERY_LOW_MV 3500
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

/* USB plugged in is always OK. Else ADC (GP29) or a detector GPIO. */
bool batteryVoltageLow();

/* 0–100, or -1 if no ADC sample yet. USB mounted reports 100. */
int batteryPercent();

void batteryLedGpioSetup();
void batteryLedGpioProcess();

/* Right-align percent + link letter on a 21-char OLED header (87%B). */
void appendStatusLinkBattery(std::string& bar, int width = 21);

#endif
