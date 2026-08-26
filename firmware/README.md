# GP2040-WF：有线 + 蓝牙 + 2.4G

不要家用 Wi-Fi。主控 **M487** 只打有线 8 kHz。蓝牙和 2.4G 用两颗 **ESP32-C3**，见 [esp32c3_ble](esp32c3_ble)。

| 芯片 | 干什么 |
|---|---|
| **M487** | 读按键；CON1 有线 8 kHz；UART 发给无线模块 |
| **C3 stick** | BLE HID + ESP-NOW 2.4G 发射 |
| **S3 dongle** | ESP-NOW 接收，USB 手柄（C3 不能做 USB HID） |
| 沁恒 CH585 | 认错厂商留下的，新板不要画 |

```bash
cd firmware/nuvoton_m487 && make
cd firmware/esp32c3_ble && pio run -e stick && pio run -e dongle
```
