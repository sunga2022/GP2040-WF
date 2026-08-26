# GP2040-WF 主控：新唐 M487

不用树莓派 RP2040。有线 8 kHz 的主控是 **新唐 Nuvoton M487**（M480 系列 USB HS），不是沁恒 CH585。

- **8 kHz 有线**：M487 片内 USB 2.0 High Speed。`bInterval=1` → 125 µs
- **蓝牙 / 2.4G**：另买 **M032BT**（USB FS，有线最高 1 kHz）
- **802.11 Wi-Fi**：只有 NuMaker-IoT-M487 板上的模组，不是 M487 内核

新板画 M487。根目录 GP2040-CE 只给旧 RP2040 板参考。

## 为什么是这颗

| 芯片 | 8 kHz 有线 | 蓝牙 | 2.4G / Wi-Fi | 结论 |
|---|---|---|---|---|
| **M487 / M484** | 有（USB HS） | 无 | 无（IoT 板才有模组） | **有线 8k 用这颗** |
| **M032BT** | 无（USB FS 最高 1k） | BLE 5.0 | 私有 2.4G | 无线用这颗 |
| RP2040 / Pico | 无 | 无 | 无 | 不用 |
| ESP32-S3 | 无 | 有 | Wi-Fi / ESP-NOW | 做不到 8k |
| 沁恒 CH585 | 有 | 有 | 私有 2.4G | 不是新唐，仓库里仅作旧实验 |

## 板子

1. **手柄（8 kHz）** [firmware/nuvoton_m487](nuvoton_m487)：NuMaker-PFM-M487，打游戏插 **CON1 USB HS**
2. **无线（1 kHz）** [firmware/nuvoton_m032bt](nuvoton_m032bt)：BLE / 2.4G 说明

```bash
cd firmware/nuvoton_m487
make
```
