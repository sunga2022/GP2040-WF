#include "switch_pro.h"
#include <string.h>

#define WF_DP_UP      (1u << 0)
#define WF_DP_DOWN    (1u << 1)
#define WF_DP_LEFT    (1u << 2)
#define WF_DP_RIGHT   (1u << 3)
#define WF_BTN_B1     (1u << 0)
#define WF_BTN_B2     (1u << 1)
#define WF_BTN_B3     (1u << 2)
#define WF_BTN_B4     (1u << 3)
#define WF_BTN_L1     (1u << 4)
#define WF_BTN_R1     (1u << 5)
#define WF_BTN_L2     (1u << 6)
#define WF_BTN_R2     (1u << 7)
#define WF_BTN_S1     (1u << 8)
#define WF_BTN_S2     (1u << 9)
#define WF_BTN_L3     (1u << 10)
#define WF_BTN_R3     (1u << 11)
#define WF_BTN_A1     (1u << 12)
#define WF_BTN_A2     (1u << 13)

#define RPT_USB_81   0x81
#define RPT_OUT_21   0x21
#define RPT_IN_30    0x30
#define RPT_FEATURE  0x01
#define RPT_RUMBLE   0x10
#define RPT_CONFIG   0x80

#define SUB_IDENTIFY              0x01
#define SUB_HANDSHAKE             0x02
#define SUB_BAUD_RATE             0x03
#define SUB_DISABLE_USB_TIMEOUT   0x04
#define SUB_ENABLE_USB_TIMEOUT    0x05

#define CMD_GET_STATE             0x00
#define CMD_BT_PAIR               0x01
#define CMD_DEVICE_INFO           0x02
#define CMD_SET_MODE              0x03
#define CMD_TRIGGER_BUTTONS       0x04
#define CMD_SET_SHIPMENT          0x08
#define CMD_SPI_READ              0x10
#define CMD_NFC_IR_CONFIG         0x21
#define CMD_NFC_IR_STATE          0x22
#define CMD_SET_PLAYER_LIGHTS     0x30
#define CMD_GET_PLAYER_LIGHTS     0x31
#define CMD_UNKNOWN_33            0x33
#define CMD_SET_HOME_LIGHT        0x38
#define CMD_TOGGLE_IMU            0x40
#define CMD_IMU_SENSITIVITY       0x41
#define CMD_READ_IMU              0x43
#define CMD_ENABLE_VIBRATION      0x48
#define CMD_GET_VOLTAGE           0x50

/* SPI 0x6000: factory config slice from GP2040-CE SwitchProDriver. */
static const uint8_t k_spi_6000[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF,
    0x03, 0xA0,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x02,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xE3, 0xFF, 0x39, 0xFF, 0xED, 0x01, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x09, 0x00, 0xEA, 0xFF,
    0xA1, 0xFF, 0x3B, 0x34, 0x3B, 0x34, 0x3B, 0x34,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xA4, 0x46, 0x6A, 0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A,
    0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A, 0xA4, 0x46, 0x6A,
    0xFF,
    0x1B, 0x1B, 0x1D,
    0xFF, 0xFF, 0xFF,
    0xEC, 0x00, 0x8C,
    0xEC, 0x00, 0x8C,
};

static const uint8_t k_spi_8000[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xB2, 0xA1, 0xA4, 0x46, 0x6A, 0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A,
    0xB2, 0xA1, 0x00, 0x08, 0x80, 0xA4, 0x46, 0x6A, 0xA4, 0x46, 0x6A,
};

typedef struct {
    int transport;
    int ready;
    uint8_t timestamp;
    uint8_t player;
    uint8_t input_mode;
    uint8_t mac[6];
    uint8_t inputs[10];
    uint8_t reply[WF_SWITCH_PRO_REPORT_SIZE];
    int reply_pending;
} WfSwitchPro;

static WfSwitchPro g;

static void stick_pack(uint8_t data[3], uint16_t x, uint16_t y)
{
    x &= 0x0FFFu;
    y &= 0x0FFFu;
    data[0] = (uint8_t)(x & 0xFFu);
    data[1] = (uint8_t)(((x >> 8) & 0x0Fu) | ((y & 0x0Fu) << 4));
    data[2] = (uint8_t)((y >> 4) & 0xFFu);
}

static void pack_inputs(uint8_t out[10], const WfFrame *frame)
{
    const uint32_t b = frame->buttons;
    const uint8_t d = frame->dpad;

    out[0] = 0x90; /* wired-ish connection, battery full-ish */
    out[1] = 0;
    if (b & WF_BTN_B3) {
        out[1] |= 1u;
    }
    if (b & WF_BTN_B4) {
        out[1] |= 2u;
    }
    if (b & WF_BTN_B1) {
        out[1] |= 4u;
    }
    if (b & WF_BTN_B2) {
        out[1] |= 8u;
    }
    if (b & WF_BTN_R1) {
        out[1] |= 0x40u;
    }
    if (b & WF_BTN_R2) {
        out[1] |= 0x80u;
    }
    out[2] = 0x80; /* charging grip */
    if (b & WF_BTN_S1) {
        out[2] |= 1u;
    }
    if (b & WF_BTN_S2) {
        out[2] |= 2u;
    }
    if (b & WF_BTN_R3) {
        out[2] |= 4u;
    }
    if (b & WF_BTN_L3) {
        out[2] |= 8u;
    }
    if (b & WF_BTN_A1) {
        out[2] |= 0x10u;
    }
    if (b & WF_BTN_A2) {
        out[2] |= 0x20u;
    }
    out[3] = 0;
    if (d & WF_DP_DOWN) {
        out[3] |= 1u;
    }
    if (d & WF_DP_UP) {
        out[3] |= 2u;
    }
    if (d & WF_DP_RIGHT) {
        out[3] |= 4u;
    }
    if (d & WF_DP_LEFT) {
        out[3] |= 8u;
    }
    if (b & WF_BTN_L1) {
        out[3] |= 0x40u;
    }
    if (b & WF_BTN_L2) {
        out[3] |= 0x80u;
    }
    stick_pack(out + 4, (uint16_t)(frame->lx >> 4), (uint16_t)(0x0FFFu - (frame->ly >> 4)));
    stick_pack(out + 7, (uint16_t)(frame->rx >> 4), (uint16_t)(0x0FFFu - (frame->ry >> 4)));
}

static void queue_reply(const uint8_t *data)
{
    memcpy(g.reply, data, WF_SWITCH_PRO_REPORT_SIZE);
    g.reply_pending = 1;
}

static void fill_subcommand_header(uint8_t *report)
{
    memset(report, 0, WF_SWITCH_PRO_REPORT_SIZE);
    report[0] = RPT_OUT_21;
    report[1] = g.timestamp;
    memcpy(report + 2, g.inputs, sizeof(g.inputs));
}

static void pack_identify(uint8_t *report)
{
    uint8_t i;
    memset(report, 0, WF_SWITCH_PRO_REPORT_SIZE);
    report[0] = RPT_USB_81;
    report[1] = SUB_IDENTIFY;
    report[3] = 0x03; /* Pro Controller */
    for (i = 0; i < 6; i++) {
        report[4 + i] = g.mac[5 - i];
    }
}

static void spi_read(uint8_t *dest, uint32_t address, uint8_t size)
{
    const uint8_t *src = NULL;
    uint32_t base = 0;
    uint32_t off;
    uint8_t n;

    memset(dest, 0xFF, size);
    if ((address & 0xFFFFFF00u) == 0x6000u) {
        src = k_spi_6000;
        base = 0x6000u;
        n = (uint8_t)sizeof(k_spi_6000);
    } else if ((address & 0xFFFFFF00u) == 0x8000u) {
        src = k_spi_8000;
        base = 0x8000u;
        n = (uint8_t)sizeof(k_spi_8000);
    } else {
        return;
    }
    off = address - base;
    if (off >= n) {
        return;
    }
    if (size > (uint8_t)(n - off)) {
        size = (uint8_t)(n - off);
    }
    memcpy(dest, src + off, size);
}

static void handle_config(uint8_t sub)
{
    uint8_t report[WF_SWITCH_PRO_REPORT_SIZE];
    memset(report, 0, sizeof(report));
    switch (sub) {
    case SUB_IDENTIFY:
        pack_identify(report);
        break;
    case SUB_HANDSHAKE:
        report[0] = RPT_USB_81;
        report[1] = SUB_HANDSHAKE;
        break;
    case SUB_BAUD_RATE:
        report[0] = RPT_USB_81;
        report[1] = SUB_BAUD_RATE;
        break;
    case SUB_DISABLE_USB_TIMEOUT:
        report[0] = RPT_IN_30;
        report[1] = sub;
        g.ready = 1;
        break;
    default:
        report[0] = RPT_IN_30;
        report[1] = sub;
        if (sub == SUB_ENABLE_USB_TIMEOUT) {
            g.ready = 1;
        }
        break;
    }
    queue_reply(report);
}

static void handle_feature(const uint8_t *data, uint16_t len)
{
    uint8_t report[WF_SWITCH_PRO_REPORT_SIZE];
    uint8_t cmd;
    uint32_t addr;
    uint8_t n;

    if (len < 11) {
        return;
    }
    cmd = data[10];
    fill_subcommand_header(report);

    switch (cmd) {
    case CMD_GET_STATE:
        report[13] = 0x80;
        report[14] = cmd;
        report[15] = 0x03;
        break;
    case CMD_BT_PAIR:
        report[13] = 0x81;
        report[14] = cmd;
        report[15] = 0x03;
        break;
    case CMD_DEVICE_INFO:
        report[13] = 0x82;
        report[14] = 0x02;
        report[15] = 0x04;
        report[16] = 0x91;
        report[17] = 0x03;
        report[18] = 0x02;
        memcpy(report + 19, g.mac, 6);
        report[25] = 0x01;
        report[26] = 0x02;
        break;
    case CMD_SET_MODE:
        g.input_mode = data[11];
        report[13] = 0x80;
        report[14] = 0x03;
        report[15] = g.input_mode;
        if (g.input_mode == 0x30 || g.input_mode == 0x3F) {
            g.ready = 1;
        }
        break;
    case CMD_TRIGGER_BUTTONS:
        report[13] = 0x83;
        report[14] = 0x04;
        break;
    case CMD_SPI_READ:
        addr = ((uint32_t)data[14] << 24) | ((uint32_t)data[13] << 16)
             | ((uint32_t)data[12] << 8) | data[11];
        n = data[15];
        report[13] = 0x90;
        report[14] = data[10];
        report[15] = data[11];
        report[16] = data[12];
        report[17] = data[13];
        report[18] = data[14];
        report[19] = data[15];
        spi_read(&report[20], addr, n);
        break;
    case CMD_SET_PLAYER_LIGHTS:
        g.player = data[11];
        report[13] = 0x80;
        report[14] = cmd;
        break;
    case CMD_GET_PLAYER_LIGHTS:
        report[13] = 0xB0;
        report[14] = cmd;
        report[15] = g.player;
        break;
    case CMD_GET_VOLTAGE:
        report[13] = 0xD0;
        report[14] = 0x50;
        report[15] = 0x83;
        report[16] = 0x06;
        break;
    default:
        report[13] = 0x80;
        report[14] = cmd;
        report[15] = 0x03;
        break;
    }
    queue_reply(report);
}

void wf_switch_pro_init(int transport)
{
    memset(&g, 0, sizeof(g));
    g.transport = transport;
    g.input_mode = 0x30;
    g.mac[0] = 0x7C;
    g.mac[1] = 0xBB;
    g.mac[2] = 0x8A;
    g.mac[3] = 0x20;
    g.mac[4] = 0x40;
    g.mac[5] = 0x01;
    g.inputs[0] = 0x90;
    g.inputs[2] = 0x80;
    stick_pack(g.inputs + 4, 0x07FF, 0x07FF);
    stick_pack(g.inputs + 7, 0x07FF, 0x07FF);
    /* Bluetooth has no USB 0x80 handshake; Switch talks 0x01 subcommands. */
    g.ready = (transport == WF_SWITCH_BT);
}

void wf_switch_pro_apply_frame(const WfFrame *frame)
{
    pack_inputs(g.inputs, frame);
}

void wf_switch_pro_on_output(uint8_t report_id, const uint8_t *data, uint16_t len)
{
    const uint8_t *p = data;
    uint8_t tmp[WF_SWITCH_PRO_REPORT_SIZE];
    uint8_t id;

    if (data == NULL || len == 0) {
        return;
    }
    id = data[0];
    if (id != RPT_FEATURE && id != RPT_RUMBLE && id != RPT_CONFIG) {
        if (report_id == RPT_FEATURE || report_id == RPT_RUMBLE || report_id == RPT_CONFIG) {
            memset(tmp, 0, sizeof(tmp));
            tmp[0] = report_id;
            memcpy(tmp + 1, data, len < (WF_SWITCH_PRO_REPORT_SIZE - 1) ? len : (WF_SWITCH_PRO_REPORT_SIZE - 1));
            p = tmp;
            id = report_id;
            len = (uint16_t)(len + 1);
        }
    }
    if (id == RPT_CONFIG && len > 1) {
        handle_config(p[1]);
    } else if (id == RPT_FEATURE) {
        handle_feature(p, len);
    }
}

int wf_switch_pro_next_report(uint8_t out[WF_SWITCH_PRO_REPORT_SIZE])
{
    if (g.reply_pending) {
        memcpy(out, g.reply, WF_SWITCH_PRO_REPORT_SIZE);
        g.reply_pending = 0;
        g.timestamp = (uint8_t)(g.timestamp + 1);
        return 1;
    }
    if (g.transport == WF_SWITCH_USB && !g.ready) {
        pack_identify(out);
        g.timestamp = (uint8_t)(g.timestamp + 1);
        return 1;
    }
    memset(out, 0, WF_SWITCH_PRO_REPORT_SIZE);
    out[0] = RPT_IN_30;
    out[1] = g.timestamp;
    memcpy(out + 2, g.inputs, sizeof(g.inputs));
    out[12] = 0x09;
    g.timestamp = (uint8_t)(g.timestamp + 1);
    return 1;
}

int wf_switch_pro_is_ready(void)
{
    return g.ready;
}
