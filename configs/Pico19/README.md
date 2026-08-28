# GP2040-WF Pico19

只保留这一套脚位。FN 已取消。

## 按键

| GPIO | 功能 |
|------|------|
| 15 | UP |
| 19 | DOWN |
| 18 | RIGHT |
| 20 | LEFT |
| 14 | B1 |
| 10 | B2 |
| 9 | R2 |
| 8 | L2 |
| 12 | B3 |
| 5 | B4 |
| 6 | R1 |
| 7 | L1 |
| 27 | S1 SELECT |
| 17 | S2 START |
| 21 | L3 |
| 13 | R3 |
| 22 | A1 |
| 26 | A2 TOUCH |
| 23 | TURBO |

## 外设

| GPIO | 功能 |
|------|------|
| 0 / 1 | OLED SDA / SCL |
| 2 / 3 | USB 认证口 D− / D+（`USB_PERIPHERAL_PIN_DPLUS 3`，`ORDER 1`） |
| 28 | WS2812，灯序只跟线，不跟键值 |
| 24 | 杰里 UART1 TX |
| 25 / 4 / 11 / 16 | XN297L CE / CSN / SCK / MOSI（Si24R1 4×4 电兼容） |
| 29 | 电量 ADC（100k/100k 分压） |

OLED 右上角：`87%B` 电量百分比 + **L** 插线 / **B** 蓝牙 / **G** 2.4G。灯串后面再多一颗 WS2812：正常绿，低电红。

## 开机模式（按住再上电，A=B1）

A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘

PS4 用板载 `PS4Auth/` 三文件。PS5 / Xbox One 走认证口 NXP7105。Xbox 360 默认已认证。

## 快捷键（无 FN）TOUCH=A2 START=S2 SELECT=S1

- A2+START+LEFT 左摇杆
- A2+START+RIGHT 右摇杆
- A2+START+UP 十字键
- A2+SELECT+UP 上优先
- A2+SELECT+LEFT 后输入优先
- A2+SELECT+DOWN 回中
- A2+SELECT+RIGHT 先输入优先

RGB 灯序只跟 WS2812 走线，不跟键值。网页里把 ABCD 改成 BCDA，按 A 仍亮原来那颗灯。
