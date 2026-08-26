# 三模：GP2040-CE 原样 + 蓝牙 + 2.4G

主控还是 **RP2040**，固件还是仓库根目录这套 **GP2040-CE**。不换 MCU，不重写手柄协议。

| 模式 | 做法 |
|------|------|
| 有线 | 手柄 USB。XInput / Switch / PS 描述符全是 CE 原来那套 |
| 蓝牙 | 插件 `WirelessAddon` 1 Mbps UART → **杰里 AC632N** |
| 2.4G | 同一插件 bitbang SPI → **Si24R1 / nRF24** → 第二块 Pico 接收器 USB HID |

USB 已经枚举（`tud_mounted()`）时不发无线。

## 手柄接线

### Pico16（完整三模）

| 功能 | Pico16 GPIO |
|------|-------------|
| 杰里 UART RX | **GP0** UART0 TX，1 Mbps 8N1 |
| Si24R1 CE | **GP1** |
| Si24R1 CSN | **GP21** |
| Si24R1 SCK | **GP25** |
| Si24R1 MOSI | **GP29** |
| Si24R1 MISO | 手柄发射端可不接 |

GP21 / GP29 原先是重复的 L3 / UP，让给射频。L3 仍在 GP2，方向仍在 GP20。

### G2（有线 + 蓝牙）

G2 按键占满了脚，只开了 UART：

| 功能 | G2 GPIO |
|------|---------|
| 杰里 UART RX | **GP24** UART1 TX，1 Mbps 8N1 |

要 2.4G 请用 Pico16，或自己在 `BoardConfig.h` 里腾 4 根脚给 `WIRELESS_NRF24_*`。

### 杰里 AC632N

- Pico TX → AC632N UART RX，共地，3.3 V
- 官方 SDK：https://github.com/Jieli-Tech/fw-AC63_BT_SDK
- 工程：`apps/hid/board/bd19/AC632N_hid.cbp`
- 打开 `TCFG_USER_BLE_ENABLE` 和 `TCFG_USER_EDR_ENABLE`
- UART 中断里调 `wf_jieli_on_byte`
- 源码：[`firmware/jieli_ac632n/`](../firmware/jieli_ac632n/)

## 2.4G 接收器（第二块 Pico）

和 Pico16 发射脚对齐，接收器多一根 MISO：

| nRF24 / Si24R1 | 接收器 Pico |
|----------------|-------------|
| CE | GP1 |
| CSN | GP21 |
| SCK | GP25 |
| MOSI | GP29 |
| MISO | **GP16** |
| VCC / GND | 3V3 / GND |

空中包：20 字节 `WfFrame`，频道 80（2480 MHz），管道 `{E7 E7 E7 E7 57}`，2 Mbps，无 ACK。协议见 `wireless/protocol.h`。

手柄拔掉 USB 之后才发 2.4G。接收器一直插电脑。

## 这不是什么

- 不是 U 盘外形、不是 CH32、不是新唐 M487
- 不是 Xbox / PS / Switch **主机蓝牙** 伪装
- 杰里自家 2.4G 跟 Si24R1 不通，2.4G 必须走 nRF24 这一路
