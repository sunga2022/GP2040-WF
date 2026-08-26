# GP2040-WF

**sunga2022** 的格斗板。主控是 **新唐 M487**，蓝牙用外挂模块，不是树莓派，也不是沁恒。

## 怎么打

| 模式 | 谁干活 | 轮询 |
|---|---|---|
| **有线** | M487 USB 2.0 High Speed（CON1） | **8 kHz** |
| **蓝牙** | 外挂 ESP32-C3（或 nRF52840） | 约 133 Hz（BLE 最快约 7.5 ms） |

M487 没有片内蓝牙。UART1（Arduino D0/D1，1 Mbps）把按键帧送给模块。CON1 插上时 PA.11 拉高，模块不发 BLE，避免一台电脑两只手柄。

## 买什么

1. [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)
2. [ESP32-C3 SuperMini](https://www.espressif.com/en/products/socs/esp32-c3) 当蓝牙模组

```bash
cd firmware/nuvoton_m487 && make test && make
cd firmware/esp32c3_ble && pio run
```

有线：ICE 口烧录，**CON1** 打游戏。蓝牙：接线见 `firmware/nuvoton_m487/include/wf_pins.h` 和 `firmware/esp32c3_ble/README.md`，电脑搜 `GP2040-WF`。

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
