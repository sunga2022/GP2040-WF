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

手柄射频用 **XN297L QFN20 3×3 mm**（磐启，nRF24 寄存器兼容）。Si24R1 / BK2425 是 **4×4**，同一套 SPI，封装要另画。

PS4：板载 `configs/Pico19/PS4Auth/` 三文件编进固件。PS5 / Xbox One：认证口 NXP7105。Xbox 360：默认已认证。无线只报 Switch。

## 2.4G 接收器（要小）

不要用 RP2040（QFN-56 7×7 + W25Q16）。A7105 是 4×4 且没有 USB。

| 芯片 | 封装 | 干什么 |
|------|------|--------|
| **XN297L** | QFN20 **3×3** | 2.4G，和手柄同一套 ShockBurst 协议 |
| **CH32X035F8U6** | QFN20 **3×3** | USB FS，WCHISP 能烧 |

板子 USB-A **22×12 mm**。CH32：CE=PA0 CSN=PA1 SCK=PA2 MOSI=PA3 MISO=PA4，USB D−/D+=PC16/PC17。烧录文件 `firmware-bin/GP2040-WF_receiver.hex`。
