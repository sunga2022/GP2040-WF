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

## 2.4G 接收器（键盘接收器那种一只芯片）

不要用 RP2040（QFN-56 7×7 + W25Q16）。不要用两颗 3×3（CH32 + XN297L）。CH582 更好买，但射频不是 nRF24，手柄还得再焊一颗。

| 芯片 | 封装 | 干什么 |
|------|------|--------|
| **nRF52820-QDAA** | QFN40 **5×5** | USB FS + 2.4G 同一颗。立创 C3015612 |

板子 USB-A **22×12 mm**。VBUS 脚直接 5V（VDDH 短到 VBUS），D−/D+=脚 12/13，ANT=脚 24，32 MHz 晶振脚 28/29。烧录走 SWD（脚 19/20），文件 `firmware-bin/GP2040-WF_receiver.hex`。手柄射频仍是 XN297L，不用改。
