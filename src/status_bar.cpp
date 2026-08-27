#include "status_bar.h"

#include "addons/wireless.h"
#include "hardware/gpio.h"
#include "tusb.h"

char statusLinkLetter()
{
    if (tud_mounted()) {
        return 'L';
    }

#if WIRELESS_ENABLED
    if (isValidPin(WIRELESS_UART_TX_PIN)) {
        return 'B';
    }
    if (isValidPin(WIRELESS_NRF24_CE_PIN)
            && isValidPin(WIRELESS_NRF24_CSN_PIN)
            && isValidPin(WIRELESS_NRF24_SCK_PIN)
            && isValidPin(WIRELESS_NRF24_MOSI_PIN)) {
        return 'G';
    }
#endif

    return 'L';
}

bool batteryVoltageLow()
{
    /* Line power / charging: never show empty. */
    if (tud_mounted()) {
        return false;
    }

    if (!isValidPin(BATTERY_LOW_PIN)) {
        return false;
    }

    /* Voltage detector, open-drain, active-low when VBAT is below threshold. */
    return gpio_get(BATTERY_LOW_PIN) == 0;
}

void batteryLedGpioSetup()
{
    if (isValidPin(BATTERY_LOW_PIN)) {
        gpio_init(BATTERY_LOW_PIN);
        gpio_set_dir(BATTERY_LOW_PIN, GPIO_IN);
        gpio_pull_up(BATTERY_LOW_PIN);
    }
    if (isValidPin(BATTERY_LED_RED_PIN)) {
        gpio_init(BATTERY_LED_RED_PIN);
        gpio_set_dir(BATTERY_LED_RED_PIN, GPIO_OUT);
        gpio_put(BATTERY_LED_RED_PIN, 0);
    }
    if (isValidPin(BATTERY_LED_GREEN_PIN)) {
        gpio_init(BATTERY_LED_GREEN_PIN);
        gpio_set_dir(BATTERY_LED_GREEN_PIN, GPIO_OUT);
        gpio_put(BATTERY_LED_GREEN_PIN, 1);
    }
}

void batteryLedGpioProcess()
{
    if (!isValidPin(BATTERY_LED_RED_PIN) && !isValidPin(BATTERY_LED_GREEN_PIN)) {
        return;
    }

    const bool low = batteryVoltageLow();
    if (isValidPin(BATTERY_LED_RED_PIN)) {
        gpio_put(BATTERY_LED_RED_PIN, low ? 1 : 0);
    }
    if (isValidPin(BATTERY_LED_GREEN_PIN)) {
        gpio_put(BATTERY_LED_GREEN_PIN, low ? 0 : 1);
    }
}

void appendStatusLinkBattery(std::string& bar, int width)
{
    char tail[2];
    tail[0] = statusLinkLetter();
    tail[1] = '\0';

    const int tlen = 1;
    if (width < tlen) {
        bar.assign(tail);
        return;
    }

    const int room = width - tlen;
    if ((int)bar.size() > room) {
        bar.resize(room);
    } else {
        bar.append(room - (int)bar.size(), ' ');
    }
    bar.append(tail);
}
