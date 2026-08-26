# 新唐 M487 + 外挂蓝牙模块

M487 负责按键和有线 **USB HS 8 kHz**。蓝牙是外面再加一颗 3.3 V 模块，不是 M487 片内的。

```
按键 ──► M487 ── USB HS CON1 ──► 电脑   8 kHz
            │
            └── UART1 1 Mbps ──► ESP32-C3 / nRF52840 ── BLE HID  ≈133 Hz
```

## 买什么

1. [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)
2. **ESP32-C3 SuperMini**（或 C3-12F 模块）当蓝牙模组，说明见 [../esp32c3_ble](../esp32c3_ble)

## 接线

| M487 | 蓝牙模块 |
|---|---|
| D1 PB.3 TX | 模块 RX（C3 GPIO4） |
| D0 PB.2 RX | 模块 TX（C3 GPIO5，可空） |
| NU7.7 PA.11 | 模块 GPIO2：CON1 枚举后拉高，BLE 闭嘴 |
| 3V3 / GND | 3V3 / GND |

按键仍接 Arduino D2–D13、A0–A5，见 `include/wf_pins.h`。

## 编译 M487

```bash
sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
cd firmware/nuvoton_m487
make test
make
```

Nu-Link ICE 烧 `build/controller.hex`。打有线插 **CON1**。打蓝牙拔 CON1，搜 `GP2040-WF`。

`bsp/` 是 [M480BSP](https://github.com/OpenNuvoton/M480BSP) 精简子集（Apache-2.0）。USB 路径改自官方 `HSUSBD_HID_Mouse`。
