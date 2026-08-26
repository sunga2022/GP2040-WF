# GP2040-WF 主控（不再使用 RP2040）

有线 + 蓝牙 + 2.4G 三模主控改成 **沁恒 CH585**。RP2040 / GP2040-CE 源码仍在仓库根目录，只作旧板参考，新板不要再画 Pico。

## 选哪块板

| 目标 | 开发板 | USB | 射频 | 轮询 |
|---|---|---|---|---|
| **三模 + 有线 8 kHz（推荐）** | **CH585M-EVT × 2** | High Speed | BLE 5.4 + 私有 2.4G | 有线/2.4G **8 kHz**，BLE ~100 Hz |
| 只有有线 8k | CH32V307、Teensy 4.1、WeAct STM32H743 | High Speed | 无 | 8 kHz，没有蓝牙/2.4G |
| 旧三模 1k | ESP32-S3-DevKitC-1 × 2 | Full Speed | BLE + ESP-NOW | 有线/2.4G **1 kHz** |
| 不要买 | Pico / Pico 2 / Pico W、STM32H723、XIAO ESP32-C3 | 无 HS 或脚太少 | — | 不适合本方案 |

CH585 是目前能在 **同一颗 MCU** 上同时做 USB HS 8k 和 2.4G/BLE 的沁恒料。CH32V307 只有 USB HS。ESP32-S3 只有 Full Speed。

## 三模：CH585

两块 [CH585M-EVT](https://www.wch-ic.com/products/CH585.html)：

1. **手柄** `firmware/ch585` `make controller`：读 GPIO，USB HS HID 8 kHz；拔掉 USB 后走 2.4G（需 RF ROM）
2. **接收器** `make dongle`：插电脑，收 2.4G，USB HS HID 8 kHz

插着手柄 USB HS 就是有线；插着接收器就是 2.4G；手机搜 `GP2040-WF` 就是蓝牙（MounRiver HID 工程）。不要把手柄 USB 和接收器同时插同一台 PC。

```bash
cd firmware/ch585
make
```

按键脚在 `firmware/ch585/include/wf_pins.h`，低电平有效。下载走 USB FS + BOOT；打游戏走 USB HS。

## 旧方案

- ESP32-S3 三模 1 kHz：`firmware/esp32s3/`
- Teensy 4.1 有线 8k：`firmware/teensy41/wired_8k/`（没有射频）
