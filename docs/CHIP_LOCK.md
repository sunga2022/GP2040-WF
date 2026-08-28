# 三模脚位（只保留 Pico19）

主控 RP2040。FN 取消。

| 功能 | GPIO |
|------|------|
| OLED SDA / SCL | 0 / 1 |
| USB 认证 D− / D+ | 2 / 3（`DPLUS 3` `ORDER 1`） |
| Si24R1 CSN / SCK / MOSI / CE | 4 / 11 / 16 / 25 |
| 按键 | 见 `configs/Pico19/BoardConfig.h` |
| 杰里 UART TX | 24（UART1，1 Mbps） |
| WS2812 | 28 |
| VBAT ADC | 29（100k/100k） |

PS4：板载 `configs/Pico19/PS4Auth/` 三文件编进固件。PS5 / Xbox One：认证口 NXP7105。Xbox 360：默认已认证。无线只报 Switch。
