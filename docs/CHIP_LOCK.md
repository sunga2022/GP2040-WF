# GP2040-WF 芯片与方案锁定

**状态：已锁定。先按本文买板、对原理图，再写固件。**

手柄无线用 **杰里 AC632N**，不用 nRF52840。nRF 没有经典蓝牙；杰里 AC63 官方 SDK 自带 BLE + EDR HID。

三模只这三件事，没有家用 Wi‑Fi：

1. 有线 USB
2. 蓝牙（BLE + 经典蓝牙）
3. 2.4G USB-A U 盘接收器

---

## 锁定的料

| 角色 | 芯片（写进 BOM） | 形态 | 现在怎么买 | 干什么 |
|---|---|---|---|---|
| 手柄主控 | **新唐 M487JIDAE** | LQFP128 | [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/) | 按键；**CON1 USB HS 8 kHz**；UART 送给杰里；SPI 发给 Si24R1 |
| 手柄无线 | **杰里 AC632N**（板级 bd19） | 量产看引脚：评估板同系列，如 **AC6321A**；SOP8 的 AC6328A 太少脚，只够极简 UART | 淘宝 **「AC632N 开发板」**；SDK：[fw-AC63_BT_SDK](https://github.com/Jieli-Tech/fw-AC63_BT_SDK)，HID 文档 [doc.zh-jieli.com/AC63](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/hid/index.html) | **BLE HID** + **经典蓝牙 EDR HID** |
| 手柄 2.4G 发射 | **Si24R1** | QFN20 4×4 mm，贴在手柄上 | 邮票孔板先焊到 M487 SPI | nRF24 兼容包，发给 U 盘 |
| **2.4G 接收器 MCU** | **沁恒 CH32V305GBU6** | QFN28，USB HS | **CH32V307V-EVT** | USB HID **4 kHz** |
| **2.4G 接收器射频** | **Si24R1** | 同一颗贴片 | 焊到 EVT SPI | 接收 |

```
按键 ──► M487 ── CON1 USB HS ──► 电脑                         8 kHz
            │
            ├── UART1 ──► AC632N ── BLE ──► 手机 / 电脑
            │                 └── EDR ──► 电脑（经典蓝牙 HID）
            └── SPI  ──► Si24R1 ──► 空中 ──► Si24R1 + CH32V305 U 盘   4 kHz
```

CON1 枚举后 PA.11 拉高：AC632N 停广播，Si24R1 停发。

---

## 为什么手柄无线改杰里、不盯 nRF52840

| | nRF52840 | **杰里 AC632N** |
|---|---|---|
| 经典蓝牙 EDR | **没有** | **有**。官方 HID demo：鼠标/键盘/手柄，`TCFG_USER_EDR_ENABLE` |
| BLE HID | 很强（iOS 最稳） | 官方也有 BLE HID，国内手柄常用。iOS 手柄兼容性一般不如 Nordic，可接受 |
| 和 LiteEMF | 接不上 EDR | 射频对得上 LiteEMF 的 `TR_EDR`；以后若接中间件，也是这颗，不是 nRF |
| 价钱 / 板 | 模组贵 | 芯片便宜，开发板淘宝现成，Code::Blocks + 官方配置工具 |
| 私有 2.4G | 可对 Si24R1/nRF24 | 杰里自己的 2.4G（`CFG_RF_24G_CODE_ID`）**对不上** Si24R1 |

所以 2.4G U 盘 **不要** 用杰里私有 2.4G（对端只能再买一颗杰里，而且杰里 USB 是 FS，只有 1 kHz）。4 kHz 仍是 **CH32V305 USB HS + 两颗 Si24R1**。手柄上 Si24R1 由 **M487 SPI** 发，杰里只打蓝牙。

杰里 USB 也不要拿来做有线 8 kHz。

---

## 开发板（先买，不要先画板）

1. NuMaker-PFM-M487  
2. **AC632N 开发板**（官方 HID 工程 `apps/hid/board/bd19/AC632N_hid.cbp`）  
3. **CH32V307V-EVT** + 两块 Si24R1 邮票孔（一块手柄、一块接收器）

不要买 ESP32-S3/C3 当蓝牙：没有经典蓝牙。不要买 nRF52840 开发板。

---

## 蓝牙能连什么（杰里）

| 连什么 | 行不行 | 原因 |
|---|---|---|
| Windows / 安卓 BLE 或经典蓝牙 HID | **行** | AC632N 双模 HID |
| iPhone BLE HID | **多数行** | 标准 BLE HID；个别 iOS 游戏比 Nordic 挑 |
| 电脑经典蓝牙 | **行** | EDR HID |
| **Xbox 主机** | **不行** | 不认普通经典蓝牙手柄 |
| **Switch / PS4 / PS5** | **射频能对上**，本仓库 **不做** 官方仿冒。PS 还要加密芯片。要成品主机蓝牙仍买山弯 BM769P |

---

## 评估板接线

### M487 ↔ AC632N

| NuMaker-PFM-M487 | AC632N 开发板 | 作用 |
|---|---|---|
| D1 **PB.3** UART1_TXD | UART RX（板子丝印，常见 PA 口，以 DEMO 原理图为准） | 1 Mbps `WfFrame` |
| NU7.7 **PA.11** | 一个 GPIO | CON1 插上后无线静音 |
| 3V3 / GND | 3V3 / GND | 共 3.3 V |

### M487 ↔ 手柄 Si24R1

SPI（SCK/MOSI/MISO）+ CSN + CE + IRQ。具体脚写程序时钉到空闲 SPI。CE 可和 PA.11 一起静音。

### 接收器

CH32V307V-EVT **USB 高速口 P6** 插电脑。Si24R1 接 EVT SPI。

同一台电脑：有线、蓝牙、2.4G 不要同时连。

---

## 仓库（程序先停）

| 路径 | 处理 |
|---|---|
| `firmware/nuvoton_m487/` | 主控，保留。UART 对端改为杰里；以后加 SPI→Si24R1 |
| `wireless/nrf52840/` | **废弃。** 无线改 AC632N |
| 杰里固件 | **尚未建目录。** 基于官方 `apps/hid`，UART 收帧后 BLE/EDR HID |
| 接收器 | CH32V305 USBHS 4 kHz + Si24R1 RX，尚未建 |
| `firmware/esp32c3_ble/` | 废弃 |
| `firmware/ch585/` | 废弃 |

确认 BOM 后再写：M487；AC632N HID；CH32V305+Si24R1。
