# GP2040-WF

**sunga2022** 的三模格斗板：有线、蓝牙、2.4G。不要家用 Wi-Fi。主控 **新唐 M487**，无线外挂两颗 ESP32-C3。

| 模式 | 谁干活 | 轮询 |
|---|---|---|
| **有线** | M487 USB HS，插 CON1 | **8 kHz** |
| **蓝牙** | 手柄上的 C3，BLE HID `GP2040-WF` | 约 133 Hz |
| **2.4G** | 手柄 C3 发 ESP-NOW，另一颗 C3 当 USB 接收器 | 约 1 kHz |

M487 没有片内射频。CON1 插上时 PA.11 拉高，无线停掉。2.4G 是接收器，不连路由器。

## 买什么

1. [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)
2. 一块 ESP32-C3 SuperMini 焊在手柄上（蓝牙 + 2.4G 发射）
3. 一块 ESP32-S3 SuperMini 当 2.4G USB 接收器（C3 的 USB 不能做 HID）

```bash
cd firmware/nuvoton_m487 && make test && make
cd firmware/esp32c3_ble && pio run
```

接线见 `firmware/esp32c3_ble/README.md`。有线走 CON1；蓝牙搜名字；2.4G 插 dongle。

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
