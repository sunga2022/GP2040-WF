# 新唐 M487 + 外挂三模无线

M487 负责按键和有线 **USB HS 8 kHz**。蓝牙和 2.4G 在外挂 ESP32-C3 上，见 [../esp32c3_ble](../esp32c3_ble)。

```
按键 ──► M487 ── CON1 USB HS ──► 电脑          8 kHz
            │
            └── UART1 ──► C3 stick ── BLE ──► 手机/电脑
                              └── ESP-NOW ──► C3 dongle USB
```

## 买什么

1. [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)
2. 一块 ESP32-C3 SuperMini（手柄）+ 一块 ESP32-S3（2.4G 接收器）

## 接线

| M487 | C3 stick |
|---|---|
| D1 PB.3 TX | GPIO4 RX |
| D0 PB.2 RX | GPIO5 TX（可空） |
| NU7.7 PA.11 | GPIO2：CON1 枚举后无线停 |
| 3V3 / GND | 3V3 / GND |

按键接 Arduino D2–D13、A0–A5，见 `include/wf_pins.h`。

## 编译 M487

```bash
cd firmware/nuvoton_m487
make test
make
```

ICE 烧 `build/controller.hex`。有线插 **CON1**。
