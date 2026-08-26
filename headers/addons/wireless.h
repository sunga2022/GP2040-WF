#ifndef _WirelessLink_H
#define _WirelessLink_H

#include "gpaddon.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "storagemanager.h"

#ifndef WIRELESS_ENABLED
#define WIRELESS_ENABLED 0
#endif

#ifndef WIRELESS_UART_TX_PIN
#define WIRELESS_UART_TX_PIN -1
#endif

#ifndef WIRELESS_UART_ID
#define WIRELESS_UART_ID 1
#endif

#ifndef WIRELESS_NRF24_CE_PIN
#define WIRELESS_NRF24_CE_PIN -1
#endif
#ifndef WIRELESS_NRF24_CSN_PIN
#define WIRELESS_NRF24_CSN_PIN -1
#endif
#ifndef WIRELESS_NRF24_SCK_PIN
#define WIRELESS_NRF24_SCK_PIN -1
#endif
#ifndef WIRELESS_NRF24_MOSI_PIN
#define WIRELESS_NRF24_MOSI_PIN -1
#endif
#ifndef WIRELESS_NRF24_MISO_PIN
#define WIRELESS_NRF24_MISO_PIN -1
#endif

#define WirelessName "WirelessLink"

class WirelessAddon : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void process();
    virtual void preprocess() {}
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return WirelessName; }

private:
    bool uartReady() const;
    bool nrfPinsReady() const;

    uart_inst_t *uart = nullptr;
    uint8_t seq = 0;
    uint64_t nextSendUs = 0;
    bool nrf24Ready = false;
};

#endif
