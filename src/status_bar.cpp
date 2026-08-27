#include "status_bar.h"

#include "addons/wireless.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "tusb.h"

#include <stdio.h>
#include <string.h>

static bool s_adcReady = false;

static int batteryAdcChannel()
{
    if (!isValidPin(BATTERY_ADC_PIN) || BATTERY_ADC_PIN < 26 || BATTERY_ADC_PIN > 29) {
        return -1;
    }
    return BATTERY_ADC_PIN - 26;
}

static int batteryMillivolts()
{
    const int ch = batteryAdcChannel();
    if (ch < 0 || !s_adcReady) {
        return -1;
    }

    adc_select_input((uint)ch);
    uint32_t acc = 0;
    for (int i = 0; i < 8; i++) {
        acc += adc_read();
    }
    const float vadc_mv = (acc / 8.0f) * (3300.0f / 4095.0f);
    return (int)(vadc_mv * (float)BATTERY_ADC_SCALE + 0.5f);
}

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

    const int mv = batteryMillivolts();
    if (mv >= 0) {
        return mv < BATTERY_LOW_MV;
    }

    if (!isValidPin(BATTERY_LOW_PIN)) {
        return false;
    }

    /* Voltage detector, open-drain, active-low when VBAT is below threshold. */
    return gpio_get(BATTERY_LOW_PIN) == 0;
}

int batteryPercent()
{
    if (tud_mounted()) {
        return 100;
    }

    const int mv = batteryMillivolts();
    if (mv < 0) {
        return -1;
    }
    if (mv <= BATTERY_EMPTY_MV) {
        return 0;
    }
    if (mv >= BATTERY_FULL_MV) {
        return 100;
    }
    return (mv - BATTERY_EMPTY_MV) * 100 / (BATTERY_FULL_MV - BATTERY_EMPTY_MV);
}

void batteryLedGpioSetup()
{
    const int ch = batteryAdcChannel();
    if (ch >= 0) {
        adc_init();
        adc_gpio_init(BATTERY_ADC_PIN);
        adc_select_input((uint)ch);
        s_adcReady = true;
    }

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
    char tail[8];
    const int pct = batteryPercent();
    const char link = statusLinkLetter();
    if (pct < 0) {
        snprintf(tail, sizeof(tail), "--%%%c", link);
    } else {
        snprintf(tail, sizeof(tail), "%d%%%c", pct, link);
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
