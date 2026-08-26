# GP2040-WF 三模（蓝牙 / 2.4G / 有线）

仓库 [sunga2022/GP2040-CE](https://github.com/sunga2022/GP2040-CE) 的固件已经在本仓库。有线路径仍是原来的 GP2040 USB 栈。无线是 **双 MCU**：RP2040 继续读键，nRF52840 做射频。

## 8 kHz：不能改

RP2040 USB 是 Full Speed。描述符里 `bInterval = 1` 已经是 1 ms 一帧，也就是 **1000 Hz 上限**。把数字改小不会变成 8 kHz，设备只会枚举失败。

| 路径 | 能到的速度 | 原因 |
|---|---|---|
| 有线 USB（RP2040） | **1 kHz** | USB Full Speed |
| 2.4G 接收器（nRF52840 USB） | **1 kHz** | 同样是 Full Speed |
| BLE HID | 约 **125 Hz** | 标准连接间隔 7.5 ms |
| 有线 8 kHz | **要换芯片** | USB High Speed：Teensy 4.1 / STM32H743 / CH32V307 |

格斗游戏 60 帧一帧 16.7 ms，现有 XInput 约 0.76 ms 已经远小于一帧。PS5 / Switch 也不会按 8 kHz 轮询。

## 接线（G2 默认）

```
RP2040 GP24 (UART1 TX, 1 Mbps) ----> XIAO nRF52840 D7 (RX)
GND                            ----> GND
```

手柄里放一块 nRF 当 **Controller**（收 UART，发 2.4G + BLE）。  
电脑再插一块 nRF 当 **Dongle**（收 2.4G，USB HID 1 kHz）。

插着 USB 时仍走 GP2040-CE（含 PS5 认证）。无线是并行输出，不要两根同时插同一台电脑。

## 编译 nRF

Arduino IDE：

1. 安装 **Seeed nRF52** 或 **Adafruit nRF52**，USB 栈选 TinyUSB  
2. 打开 `wireless/nrf52840/GP2040WF_Controller/GP2040WF_Controller.ino` 烧到手柄侧 XIAO  
3. 打开 `wireless/nrf52840/GP2040WF_Dongle/GP2040WF_Dongle.ino` 烧到接收器  

如果 IDE 找不到 `../protocol.h`，把 `wireless/protocol.h` 和 `wireless/nrf52840/radio_link.*` 拷进对应 sketch 目录。

其它板打开无线：在 `BoardConfig.h` 里加

```c
#define WIRELESS_ENABLED 1
#define WIRELESS_UART_TX_PIN <空闲GPIO>
#define WIRELESS_UART_ID 1   // GP4/8/20/24 用 uart1
```

并把该脚设为 `ASSIGNED_TO_ADDON`。G2 已占用 GP24。

## 开发板

**继续有线 1k（现有产品）**  
G2 / RUYI / doio / Pico16 等 RP2040 板。不要为了 8k 换掉。

**三模 1k 原型（推荐）**  
Seeed **XIAO nRF52840** × 2（手柄 + 接收器）。或 Nordic nRF52840 DK、Nice!Nano v2。

**双 MCU 保住 GP2040**  
现有 Pico/G2 + 一块 XIAO nRF52840。这是本仓库正在做的架构。

**不要用来冲 8k**  
Pico、Pico 2、Pico W、ESP32-S3、nRF52840：USB 都是 Full Speed。

**只验证有线 8k（新产品，不是本固件）**  
Teensy 4.1、WeAct STM32H743、沁恒 CH32V307。不要买 STM32H723。
