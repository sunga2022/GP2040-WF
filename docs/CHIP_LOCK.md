# 三模脚位（只保留 Pico19）

主控 RP2040。FN 取消。

| 功能 | GPIO |
|------|------|
| OLED SDA / SCL | 0 / 1 |
| USB 认证 D− / D+ | 2 / 3（`DPLUS 3` `ORDER 1`） |
| XN297L CSN / SCK / MOSI / CE | 4 / 11 / 16 / 25 |
| 按键 | 见 `configs/Pico19/BoardConfig.h` |
| 杰里 UART TX | 24（UART1，1 Mbps） |
| WS2812 | 28 |
| VBAT ADC | 29（100k/100k） |

手柄射频用 **XN297L QFN20 3×3 mm**（磐启，nRF24 寄存器兼容）。Si24R1 是 **4×4**，同一套 SPI，封装要另画。不要改手柄 2.4G。

手柄蓝牙用 **杰里 AC632N / AC6321A** 经典蓝牙。不要换成 CH582 / CH585 / PT3220 BLE，主机对不上。

PS4：板载 `configs/Pico19/PS4Auth/` 三文件编进固件。PS5 / Xbox One：认证口 NXP7105。Xbox 360：默认已认证。无线只报 Switch。插着手柄 USB 时无线静音。

## 2.4G 接收器（Si24R1 + USB MCU 两颗）

不要用 nRF52820（贵）。不要用一只带 BLE PHY 的 MCU 听 XN297L（CH582 / CH585 / PT3220 空中对不上）。不要用 RP2040 做接收器（7×7 + Flash，16.30×11.20 塞不下）。

| 芯片 | 封装 | 立创 | 干什么 |
|------|------|------|--------|
| **CH32X035F8U6** | QFN20 **3×3** | C42442062 | USB FS HID Switch Pro `057E:2009`，`bInterval=1` → **1 kHz** |
| **Si24R1** | QFN20 **4×4** P0.5 | C14436 | 2.4G RX，对齐手柄 XN297L |

板子 USB-A **16.30×11.20 mm**（略小于 12 mm USB-A 外壳内宽）。VBUS 经 LDO 到 3.3 V。烧录走 WCHISP（上电 D+ 经 4.7 kΩ 到 3V3），文件 `firmware-bin/GP2040-WF_receiver.bin`，起始地址 `0x00000000`。
