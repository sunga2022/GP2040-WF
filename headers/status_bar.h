#ifndef STATUS_BAR_H_
#define STATUS_BAR_H_

#include <string>

#ifndef BATTERY_ADC_PIN
#define BATTERY_ADC_PIN -1
#endif

#ifndef BATTERY_ADC_SCALE
#define BATTERY_ADC_SCALE 3.0f
#endif

#ifndef BATTERY_MV_EMPTY
#define BATTERY_MV_EMPTY 3300
#endif

#ifndef BATTERY_MV_FULL
#define BATTERY_MV_FULL 4200
#endif

/* 'L' wired USB, 'B' Bluetooth (Jerry UART), 'G' 2.4G (nRF). */
char statusLinkLetter();

/* 0–100, or -1 if this board has no battery ADC. */
int statusBatteryPercent();

/* Right-align battery + link letter on a 21-char OLED header. */
void appendStatusLinkBattery(std::string& bar, int width = 21);

#endif
