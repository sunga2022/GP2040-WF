# GP2040-WF 主控：一颗新塘 CH585

不用树莓派 RP2040，也不用两颗 ESP32。手柄主控就是 **一颗沁恒 CH585**：

- **8 kHz 有线**：片内 USB 2.0 High Speed
- **蓝牙**：片内 BLE 5.4
- **无线**：片内 2.4G（手柄圈说的 wifi/2.4G 接收器，不是 802.11）

新板不要再画 Pico。根目录 GP2040-CE 只给旧 RP2040 板参考。

## 为什么不是别的料

| 芯片 | 8 kHz 有线 | 蓝牙 | 2.4G / Wi-Fi | 结论 |
|---|---|---|---|---|
| **CH585** | 有（USB HS） | BLE 5.4 | 私有 2.4G，最高 8k | **用这颗** |
| CH32V307 | 有 | 无 | 无 | 只有有线 |
| RP2040 / Pico | 无（USB FS 最高 1k） | 无 | 无 | 不用 |
| ESP32-S3 | 无（USB FS 最高 1k） | 有 | Wi-Fi / ESP-NOW | 做不到 8k |
| 沁恒其它 BLE 料（CH582 等） | 无（USB FS） | 有 | 2.4G | 做不到 8k |

沁恒没有带 **家用 Wi-Fi（802.11）** 又带 USB HS 的 MCU。要 8k + 蓝牙 + 无线，CH585 的 2.4G 就是那条无线。

## 板子

两块 [CH585M-EVT](https://www.wch-ic.com/products/CH585.html)：

1. **手柄** `firmware/ch585`：GPIO + USB HS 8k；没插 USB 时发 2.4G
2. **接收器**：插电脑，收 2.4G，USB HS 8k

```bash
cd firmware/ch585
make
```

按键脚：`firmware/ch585/include/wf_pins.h`。下载走 USB FS + BOOT，打游戏走 USB HS。

蓝牙 / 2.4G 要 MounRiver + 沁恒 ROM：`firmware/ch585/mrs/README.md`。
