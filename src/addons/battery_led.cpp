#include "addons/battery_led.h"
#include "helper.h"

bool BatteryLedAddon::available()
{
    return isValidPin(BATTERY_LOW_PIN)
        || isValidPin(BATTERY_LED_RED_PIN)
        || isValidPin(BATTERY_LED_GREEN_PIN)
        || (BATTERY_LED_INDEX >= 0);
}

void BatteryLedAddon::setup()
{
    batteryLedGpioSetup();
}

void BatteryLedAddon::process()
{
    batteryLedGpioProcess();
}
