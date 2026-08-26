# GP2040-WF 主控（不再使用 RP2040）

有线 + 蓝牙 + 2.4G 三模主控改成 **ESP32-S3**。RP2040 / GP2040-CE 源码仍在仓库根目录，只作旧板参考，新板不要再画 Pico。

## 选哪块板

| 目标 | 开发板 | USB | 射频 | 轮询 |
|---|---|---|---|---|
| **三模（推荐）** | ESP32-S3-DevKitC-1 × 2 | Full Speed | BLE + ESP-NOW | 有线/2.4G **1 kHz**，BLE ~125 Hz |
| 三模、射频更好 | nRF52840 DK × 2 | Full Speed | BLE + 私有 2.4G | 同样 1 kHz 上限 |
| **有线 8 kHz** | **Teensy 4.1** | High Speed | 无 | **8 kHz**，没有蓝牙/2.4G |
| 便宜的 HS | WeAct STM32H743、CH32V307 | High Speed | 无 | 8 kHz 有线原型 |
| 不要买 | Pico / Pico 2 / Pico W、STM32H723、XIAO ESP32-C3 | 无 HS 或脚太少 | — | 不适合本方案 |

**8 kHz 和三模不能靠同一块常见开发板同时做好。** ESP32-S3 没有 USB High Speed；Teensy 4.1 没有好用的蓝牙/2.4G。要三模 8k 得 USB HS MCU + 独立射频，那是下一阶段。

## 三模：ESP32-S3

两块 [ESP32-S3-DevKitC-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html)：

1. **手柄** `firmware/esp32s3/controller`：读 GPIO，USB HID + BLE HID + ESP-NOW 发出  
2. **接收器** `firmware/esp32s3/dongle`：插电脑，收 ESP-NOW，USB HID 1 kHz  

插着手柄 USB 就是有线；插着接收器就是 2.4G；手机/平板搜 `GP2040-WF` 就是蓝牙。不要把手柄 USB 和接收器同时插同一台 PC。

```bash
pip install platformio
cd firmware/esp32s3/controller
pio run -t upload
cd ../dongle
pio run -t upload
```

按键脚在 `firmware/esp32s3/include/wf_pins.h`，低电平有效。避开 GPIO19/20（板载 USB）。

## 有线 8k：Teensy 4.1

见 `firmware/teensy41/wired_8k/`。必须改 Teensyduino 的 `JOYSTICK_INTERVAL` 为 1，否则仍是 1k。
