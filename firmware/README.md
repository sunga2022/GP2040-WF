# GP2040-WF 主控：新唐 M487 + 蓝牙模块

不用树莓派 RP2040。有线 8 kHz 是 **M487 USB HS**。蓝牙是 **外挂 ESP32-C3**（或 nRF52840），接 UART1。

| 芯片 | 干什么 |
|---|---|
| **M487** | 读按键；CON1 有线 8 kHz；UART 1 Mbps 发给模块 |
| **ESP32-C3** | 只做 BLE HID，见 [esp32c3_ble](esp32c3_ble) |
| M032BT | 新唐自己的 BLE 料，不是这个模块方案 |
| 沁恒 CH585 | 认错厂商留下的，新板不要画 |

```bash
cd firmware/nuvoton_m487 && make
cd firmware/esp32c3_ble && pio run
```
