# 新唐 M487 + 外挂 nRF52840

芯片锁定见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)。M487 只做按键和有线 **USB HS 8 kHz**。蓝牙和 2.4G 在 **nRF52840** 上，接收器也是 nRF52840。

```
按键 ──► M487 ── CON1 USB HS ──► 电脑          8 kHz
            │
            └── UART1 ──► nRF52840 ── BLE ──► 手机/电脑
                              └── ESB 2.4G ──► nRF52840 接收器 USB
```

## 买什么

1. [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)
2. 两块 [Seeed XIAO nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/)：一块焊手柄，一块当 2.4G 接收器

## 接线

| M487 | XIAO nRF52840（手柄） |
|---|---|
| D1 PB.3 TX | D7 RX |
| D0 PB.2 RX | TX（可空） |
| NU7.7 PA.11 | D2：CON1 枚举后无线停 |
| 3V3 / GND | 3V3 / GND |

按键接 Arduino D2–D13、A0–A5，见 `include/wf_pins.h`。

## 编译 M487

```bash
cd firmware/nuvoton_m487
make test
make
```

ICE 烧 `build/controller.hex`。有线插 **CON1**。
