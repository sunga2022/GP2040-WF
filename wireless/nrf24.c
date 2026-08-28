#include "nrf24.h"
#include "protocol.h"

#ifdef PICO_BOARD
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#define WF_GPIO_OUT(p) gpio_init(p); gpio_set_dir(p, GPIO_OUT)
#define WF_GPIO_IN(p)  gpio_init(p); gpio_set_dir(p, GPIO_IN); gpio_pull_up(p)
#define WF_WR(p, v)    gpio_put(p, v)
#define WF_RD(p)       gpio_get(p)
#define WF_DELAY_US(u) busy_wait_us_32(u)
#define WF_SPI_GAP()   busy_wait_at_least_cycles(12)
#elif defined(CH32X03x) || defined(CH32V20x)
#include "ch32fun.h"
#define WF_GPIO_OUT(p) funPinMode((p), GPIO_CFGLR_OUT_50Mhz_PP)
#define WF_GPIO_IN(p)  do { funPinMode((p), GPIO_CFGLR_IN_PUPD); funDigitalWrite((p), FUN_HIGH); } while (0)
#define WF_WR(p, v)    funDigitalWrite((p), (v))
#define WF_RD(p)       funDigitalRead(p)
#define WF_DELAY_US(u) Delay_Us(u)
#define WF_SPI_GAP()   Delay_Us(1)
#else
/* Host unit tests compile this file as a stub. */
#define WF_GPIO_OUT(p) (void)(p)
#define WF_GPIO_IN(p)  (void)(p)
#define WF_WR(p, v)    (void)(p), (void)(v)
#define WF_RD(p)       (0)
#define WF_DELAY_US(u) (void)(u)
#define WF_SPI_GAP()   ((void)0)
#endif

#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define NOP           0xFF

#define REG_CONFIG    0x00
#define REG_EN_AA     0x01
#define REG_EN_RXADDR 0x02
#define REG_SETUP_AW  0x03
#define REG_SETUP_RETR 0x04
#define REG_RF_CH     0x05
#define REG_RF_SETUP  0x06
#define REG_STATUS    0x07
#define REG_RX_ADDR_P0 0x0A
#define REG_TX_ADDR   0x10
#define REG_RX_PW_P0  0x11
#define REG_FIFO_STATUS 0x17
#define REG_DYNPD     0x1C
#define REG_FEATURE   0x1D

static WfNrf24Pins g_pins;
static uint8_t g_pipe[5] = WF_PIPE_ADDR;

static void csn(int v) { WF_WR(g_pins.csn, v); }
static void ce(int v) { WF_WR(g_pins.ce, v); }

static uint8_t spi_byte(uint8_t val)
{
    uint8_t read = 0;
    for (int i = 7; i >= 0; i--) {
        WF_WR(g_pins.mosi, (val >> i) & 1);
        WF_SPI_GAP();
        WF_WR(g_pins.sck, 1);
        WF_SPI_GAP();
        if (g_pins.miso >= 0 && WF_RD(g_pins.miso)) {
            read |= (uint8_t)(1u << i);
        }
        WF_WR(g_pins.sck, 0);
    }
    return read;
}

static void write_reg(uint8_t reg, uint8_t val)
{
    csn(0);
    spi_byte((uint8_t)(W_REGISTER | (reg & 0x1F)));
    spi_byte(val);
    csn(1);
}

static void write_addr(uint8_t reg, const uint8_t *addr)
{
    csn(0);
    spi_byte((uint8_t)(W_REGISTER | (reg & 0x1F)));
    for (int i = 0; i < 5; i++) {
        spi_byte(addr[i]);
    }
    csn(1);
}

static void cmd(uint8_t c)
{
    csn(0);
    spi_byte(c);
    csn(1);
}

void wf_nrf24_init(const WfNrf24Pins *pins, int is_tx)
{
    g_pins = *pins;
    WF_GPIO_OUT(g_pins.ce);
    WF_GPIO_OUT(g_pins.csn);
    WF_GPIO_OUT(g_pins.sck);
    WF_GPIO_OUT(g_pins.mosi);
    if (g_pins.miso >= 0) {
        WF_GPIO_IN(g_pins.miso);
    }
    ce(0);
    csn(1);
    WF_WR(g_pins.sck, 0);
    WF_DELAY_US(5000);

    write_reg(REG_EN_AA, 0x00);
    write_reg(REG_EN_RXADDR, 0x01);
    write_reg(REG_SETUP_AW, 0x03);
    write_reg(REG_SETUP_RETR, 0x00);
    write_reg(REG_RF_CH, (uint8_t)(WF_RADIO_CHANNEL));
    write_reg(REG_RF_SETUP, 0x0E); /* 2 Mbps, 0 dBm */
    write_reg(REG_RX_PW_P0, WF_FRAME_SIZE);
    write_reg(REG_DYNPD, 0x00);
    write_reg(REG_FEATURE, 0x00);
    write_addr(REG_RX_ADDR_P0, g_pipe);
    write_addr(REG_TX_ADDR, g_pipe);
    cmd(FLUSH_TX);
    cmd(FLUSH_RX);
    if (is_tx) {
        write_reg(REG_CONFIG, 0x0E); /* PWR_UP, CRC 2 byte, PTX */
    } else {
        write_reg(REG_CONFIG, 0x0F); /* PRX */
        ce(1);
    }
    WF_DELAY_US(2000);
}

int wf_nrf24_send(const uint8_t *data, size_t len)
{
    if (len > 32) {
        len = 32;
    }
    cmd(FLUSH_TX);
    csn(0);
    spi_byte(W_TX_PAYLOAD);
    for (size_t i = 0; i < len; i++) {
        spi_byte(data[i]);
    }
    csn(1);
    ce(1);
    WF_DELAY_US(15);
    ce(0);
    return 1;
}

int wf_nrf24_recv(uint8_t *data, size_t len)
{
    if (g_pins.miso < 0) {
        return 0;
    }
    csn(0);
    uint8_t status = spi_byte(NOP);
    csn(1);
    if ((status & 0x40) == 0) {
        return 0;
    }
    csn(0);
    spi_byte(R_RX_PAYLOAD);
    for (size_t i = 0; i < len && i < 32; i++) {
        data[i] = spi_byte(0xFF);
    }
    csn(1);
    write_reg(REG_STATUS, 0x70);
    return 1;
}
