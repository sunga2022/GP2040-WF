# GP2040-WF 芯片与方案锁定

**状态：已锁定。先按本文买板、对原理图，再写固件。**

仓库里现有的 ESP32-C3 / ESP32-S3 / CH585 代码是选型过程留下的，**新板不要按那些 BOM 做。**

三模只这三件事，没有家用 Wi‑Fi、没有 802.11、不连路由器：

1. 有线 USB
2. 蓝牙 HID
3. 2.4G USB 接收器（U 盘外形，私有 2.4G，不是 Wi‑Fi）

---

## 锁定的三颗料

| 角色 | 芯片（写进 BOM） | 封装 / 模组 | 现在怎么买 | 干什么 |
|---|---|---|---|---|
| 手柄主控 | **新唐 M487JIDAE** | LQFP128 | [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/) | 读按键；**CON1 USB HS 8 kHz**；UART 送给无线模组 |
| 手柄无线 | **Nordic nRF52840-QIAA** | 量产：**Raytac MDBT50Q-1MV2**（10.5×15.5 mm，陶瓷天线，Nordic 推荐模组） | 先用 [Seeed XIAO nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/) 验证 UART/BLE | **BLE HID**（手机/电脑兼容）+ 私有 2.4G **发射** |
| **2.4G 接收器** | **Nordic nRF52820-QDAA** | **5×5 mm QFN40**，片内 USB + 射频。自画 **USB-A 公头 U 盘小板**，PCB/陶瓷天线 | 先买 [nRF52840 Dongle PCA10059](https://www.nordicsemi.com/Products/Development-hardware/nRF52840-Dongle) 当 U 盘外形验证（射频同家族） | 私有 2.4G **接收** + **USB HID 手柄** |

```
按键 ──► M487 ── CON1 USB HS ──► 电脑                 仅此路径 8 kHz
            │
            └── UART1 ──► nRF52840 模组 ── BLE ──► 手机 / 电脑
                               └── ESB 2.4G ──► nRF52820 U 盘接收器 USB ──► 电脑
```

CON1 枚举成功后，M487 把 **PA.11** 拉高，手柄 nRF52840 **关掉 BLE 和 2.4G 发射**。

---

## 接收器：U 盘大小，小小 MCU

产品形态是 **USB-A 公头接收器**，体积按普通 U 盘做（大约 45–55 × 16–20 × 8 mm 量级）。**禁止**再塞这些东西：

- ESP32-S3 / C3 SuperMini、XIAO 开发板（USB-C 母座，不是 U 盘）
- nRF24L01+PA+LNA、SMA 胶棒天线、大金属屏蔽 2.4G 模块
- 第二颗 USB 单片机 + 外挂射频模块（两颗料、板子做不大）

接收器 **只允许一颗小 MCU**：

| 项目 | 锁定 |
|---|---|
| 料号 | **nRF52820-QDAA** |
| 为什么小 | 5×5 mm QFN40（还有 2.5 mm WLCSP，量产不够小再换）。片内 USB FS、片内 2.4G、VBUS 可直接 5 V，U 盘上几乎只需：MCU + 32 MHz 晶振 + USB-A 金手指 + PCB 天线 |
| 空中协议 | Nordic Enhanced ShockBurst，和手柄 **nRF52840 同一套射频 IP**，1/2 Mbps 可互通 |
| RAM 不够时 | 同封装改 **nRF52833-QDAA**（同样 5×5 QFN40，脚兼容，128 KB RAM）。外形不变 |

实验室先插 **PCA10059**（Nordic 官方 USB-A 小棒，已经是 U 盘插法）把协议跑通；量产再画 nRF52820 小板，把壳做成 U 盘。能买到 April Brother **USB Dongle 52820** 也可以直接当 52820 样机。

接收器 USB 是 Full Speed，电脑侧大约 **1 kHz**，不要宣传 8 kHz。

---

## 主板蓝牙 / 2.4G：兼容性优先

手柄无线 **不选小而冷门的料**，选生态最全、过认证最多的那颗：

| 要兼容什么 | 为什么是 nRF52840 + Raytac 模组 |
|---|---|
| 手机 / 平板 / Windows / macOS 蓝牙手柄 | Nordic SoftDevice + HID over GATT，iOS/Android 实际出货量最大的 BLE HID 方案之一。ESP32、新唐 M032BT、沁恒 BLE 在 iOS 上更容易对不上 |
| 和 U 盘接收器对频 | nRF52 家族空中兼容（52840 ↔ 52820 ↔ 52833，文档写明也可对 nRF24）。不要手柄走 ESP-NOW、接收器走另一家 2.4G |
| 射频法规 | Raytac **MDBT50Q-1MV2** 有 FCC / CE / SRRC / NCC 等模组认证，主板复用模组比自画 nRF52840 天线稳 |
| 体积（手柄内部） | 模组 **10.5×15.5 mm** 陶瓷天线，不是外置天线大模块 |

评估阶段 XIAO nRF52840 只负责把 UART 和 BLE 打通。量产 BOM 写成 **MDBT50Q-1MV2**（或同芯片的安信可 / 亿佰特 E73-2G4M08S1C，必须是 **nRF52840**，不要换成 C3/S3）。

---

## 明确不采用

| 候选 | 原因 |
|---|---|
| 接收器用 XIAO nRF52840 | 能发射频，但是 USB-C 开发板，做不成 U 盘 |
| 接收器用 ESP32-S3 | Wi‑Fi 模组太大，且是 Wi‑Fi PHY |
| 接收器用 nRF24L01 模块 + CH552 | 模块太大；和主板 BLE 还要再配一颗料。Si24R1 贴片可以很小，但和 nRF52840 对频不如 52820 稳 |
| 手柄无线用 ESP32-C3 | BLE HID 兼容性不如 Nordic，尤其是 iOS |
| 手柄无线用 M032BT | 新唐 BLE 栈和手机兼容面窄；接收器还要另找 USB 小料 |
| 沁恒 CH585 | 认错厂商。主控是新唐 |

---

## 速率（不要对外乱讲 8 kHz）

| 模式 | 上限 | 原因 |
|---|---|---|
| 有线（M487 CON1） | **8 kHz** | USB 2.0 High Speed，`bInterval = 1` |
| 蓝牙 | 约 **133 Hz** | BLE HID 连接间隔下限约 7.5 ms |
| 2.4G U 盘 | 约 **1 kHz** | nRF52820 USB 是 **Full Speed** |

---

## 评估板接线（锁定）

### 手柄：M487 ↔ nRF52840（先用 XIAO，量产改焊 MDBT50Q）

| NuMaker-PFM-M487 | XIAO nRF52840 | 作用 |
|---|---|---|
| D1 **PB.3** UART1_TXD | **D7 / RX** | 1 Mbps 按键帧 `WfFrame` |
| D0 PB.2 UART1_RXD | TX（可空） | 调试回读 |
| NU7.7 **PA.11** | **D2** | CON1 插上后为高，无线静音 |
| 3V3 / GND | 3V3 / GND | 共地共 3.3 V |

有线 USB 只用 **CON1**（HS）。

量产 MDBT50Q 只需把模组的 UART RX、静音 GPIO、3V3、GND 引到同样这四根网。

### 2.4G 接收器

PCA10059 或量产 nRF52820 U 盘 **只插电脑 USB-A**。不接 M487，不接手柄模组的线。两颗 nRF 只靠私有 2.4G 对上。

同一台电脑：**有线、蓝牙、2.4G 不要同时连**。拔 CON1 再用无线。

---

## 现成仓库怎么看待（程序先停）

| 路径 | 处理 |
|---|---|
| `firmware/nuvoton_m487/` | **保留。** 主控已锁定。 |
| `wireless/nrf52840/` | **手柄无线方向。** 草稿。接收器量产是 **nRF52820**，协议同一套 ESB。 |
| `firmware/esp32c3_ble/` | **废弃。** |
| `firmware/ch585/` | **废弃。** |
| `firmware/nuvoton_m032bt/` | **不采用。** |
| 根目录 GP2040-CE / Pico | **旧板。** 新板不要 RP2040。 |

确认本文之后再写程序，只做三件事：M487 有线 + UART；手柄 nRF52840 的 BLE 与 2.4G 发射；**nRF52820 U 盘**的 2.4G 接收与 USB HID。
