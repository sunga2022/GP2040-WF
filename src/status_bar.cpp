#include "status_bar.h"

#include "helper.h"
#include "addons/wireless.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "tusb.h"

#include <stdio.h>
#include <string.h>

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

int statusBatteryPercent()
{
#if BATTERY_ADC_PIN >= 26 && BATTERY_ADC_PIN <= 29
    static bool adcReady = false;
    if (!adcReady) {
        adc_init();
        adc_gpio_init(BATTERY_ADC_PIN);
        adcReady = true;
    }

    adc_select_input(BATTERY_ADC_PIN - 26);
    const uint16_t raw = adc_read();
    const float volts = (raw * 3.3f / 4095.0f) * (float)BATTERY_ADC_SCALE;
    int mv = (int)(volts * 1000.0f + 0.5f);
    if (mv <= BATTERY_MV_EMPTY) {
        return 0;
    }
    if (mv >= BATTERY_MV_FULL) {
        return 100;
    }
    return (mv - BATTERY_MV_EMPTY) * 100 / (BATTERY_MV_FULL - BATTERY_MV_EMPTY);
#else
    return -1;
#endif
}

void appendStatusLinkBattery(std::string& bar, int width)
{
    char tail[8];
    const char letter = statusLinkLetter();
    const int pct = statusBatteryPercent();
    if (pct >= 0) {
        snprintf(tail, sizeof(tail), "%d%%%c", pct, letter);
    } else {
        snprintf(tail, sizeof(tail), "--%c", letter);
    }

    const int tlen = (int)strlen(tail);
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
