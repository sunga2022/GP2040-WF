# ESP32-C3 蓝牙模块（外挂在 M487 上）

M487 是手柄主控。这颗 **ESP32-C3** 只当蓝牙模组：UART 收 `WfFrame`，发出 BLE HID 手柄。

买 **ESP32-C3 SuperMini** 或带 C3-12F 的小模块，3.3 V。不要用它做有线 8 kHz。

## 接线（杜邦线）

| NuMaker-PFM-M487 | ESP32-C3 | 作用 |
|---|---|---|
| D1 PB.3 UART1_TXD | GPIO4 RX | M487 → 模块，1 Mbps |
| D0 PB.2 UART1_RXD | GPIO5 TX | 预留 |
| NU7.7 PA.11 | GPIO2 | 高电平 = CON1 已枚举，模块不发 BLE |
| 3V3 | 3V3 | 共地供电 |
| GND | GND | |

有线插 **CON1** 时 M487 把 PA.11 拉高，电脑只走 USB HS 8 kHz。拔 CON1 后手机/电脑搜 **GP2040-WF**。BLE 连接间隔大约 7.5 ms（约 133 Hz），不是 8 kHz。

## 编译

```bash
pip install platformio
cd firmware/esp32c3_ble
pio run
pio run -t upload
```

Arduino IDE：板选 ESP32C3 Dev Module，装 [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)，把 `src/main.cpp` 当 sketch 用，并把 `wireless/` 加进头文件路径。

## 别的模块

同样的 UART 帧也能喂给 Seeed XIAO nRF52840，见 `wireless/nrf52840/GP2040WF_Controller`。
