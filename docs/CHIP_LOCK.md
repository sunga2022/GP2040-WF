# GP2040-WF 芯片与方案锁定

**状态：已锁定。先按本文买板、对原理图，再写固件。**

仓库里现有的 ESP32 / CH585 / nRF52820 接收器选型是过程稿，**新板按下面这套。**

三模只这三件事，没有家用 Wi‑Fi：

1. 有线 USB
2. 蓝牙 HID
3. 2.4G USB-A U 盘接收器

---

## 锁定的料

| 角色 | 芯片（写进 BOM） | 形态 | 现在怎么买 | 干什么 |
|---|---|---|---|---|
| 手柄主控 | **新唐 M487JIDAE** | LQFP128 | [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/) | 按键；**CON1 USB HS 8 kHz**；UART 送给无线模组 |
| 手柄无线 | **Nordic nRF52840-QIAA** | 量产 Raytac **MDBT50Q-1MV2**（10.5×15.5 mm） | 评估：[XIAO nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/) | **BLE HID**（兼容性强）+ nRF24/ESB **2.4G 发射** |
| **2.4G 接收器 MCU** | **沁恒 CH32V305GBU6** | **QFN28**，片内 **USB 2.0 High Speed PHY** | 评估：淘宝常见 **CH32V307V-EVT**（同一套 USBHS，例程最多）；也有官方 **CH32V305EVT** | USB HID **4 kHz** |
| **2.4G 射频（接收器上）** | **Si24R1** | **QFN20 4×4 mm** 贴片，PCB 天线。不是 PA 大模块 | 评估：Si24R1 邮票孔小板焊到 EVT 的 SPI | 收手柄 nRF52840 的 ESB/nRF24 包 |

```
按键 ──► M487 ── CON1 USB HS ──► 电脑                      8 kHz
            │
            └── UART1 ──► nRF52840 ── BLE ──► 手机 / 电脑     ~133 Hz
                               └── ESB 2Mbps ──► Si24R1
                                                    │
                                                    CH32V305 USB HS HID ──► 电脑   4 kHz
```

CON1 枚举后 PA.11 拉高，手柄 nRF52840 关掉 BLE 和 2.4G。

---

## 为什么接收器从 nRF52820 换成 CH32V305

| | nRF52820 | **CH32V305 + Si24R1** |
|---|---|---|
| USB | 只有 Full Speed → 电脑最多 **1 kHz** | **High Speed**，微帧 125 µs，`bInterval = 2` → **4 kHz** |
| 零售价 | 立创大约 2.8 美元一颗，还不好买开发板 | CH32V305 大约几块钱；**CH32V307V-EVT 开发板淘宝到处都是** |
| 射频 | 片内 Nordic 2.4G | 外挂 **Si24R1** 4×4 mm，和 nRF52840 的 ESB/nRF24 **空中兼容**（Nordic 文档写明） |
| U 盘体积 | 单芯片小 | MCU 是 QFN28，射频是 QFN20，仍是 U 盘料，**不要** nRF24+PA 绿板 |

4 kHz 做不到 USB Full Speed 上。CH552、nRF52 USB、ESP32-S3 USB 都是 FS，全部淘汰。

量产 U 盘：USB-A 公头 + CH32V305GBU6 + Si24R1 + 32 MHz 晶振 + PCB 天线。外壳按普通 U 盘。

Si24R1 不够稳时，同焊盘可改 **nRF24L01+** 的 QFN 原片（不是模块）。空中仍 2 Mbps、动态载荷、20 字节 `WfFrame`，4 kHz 时建议关掉 ACK，包时间才进得去 250 µs。

---

## 开发板（先买这个，不要先画 U 盘）

淘宝搜 **「CH32V307V-EVT」** 或 **「CH32V307 评估板」**（沁恒官方，带 USB **高速**口 P6）。

- USB 高速 HID 例程、MounRiver、TinyUSB 都是对着这块板写的
- CH32V305 和 CH32V307 **同一套 USBHS 外设**，评估板上跑通再缩到 QFN28 的 305
- 把 Si24R1 邮票孔板的 SPI（SCK/MOSI/MISO/CSN/CE/IRQ）接到 EVT 排针

也能买官方 **CH32V305EVT**（板上就是 GBU6），板子少一些。

不要买 CH32V203 / CH32V303 当接收器：那两颗 **没有 USB HS**，4 kHz 做不了。

---

## 主板蓝牙 / 2.4G：继续用 nRF52840

手柄无线仍要兼容性强，不换成便宜 BLE：

- iOS / Android / Windows BLE HID：Nordic SoftDevice 最稳
- 2.4G：nRF52840 可按 nRF24 兼容模式发给 Si24R1，手柄不用再加第二颗射频
- 量产模组：Raytac MDBT50Q-1MV2（有 FCC/CE/SRRC）

「兼容性强」指的是 **电脑和手机的标准 BLE 手柄**，不是主机。

| 蓝牙连什么 | 行不行 | 原因 |
|---|---|---|
| Windows / macOS / Linux、安卓、iPhone | **行**（nRF52840 BLE HID） | 标准 BLE HID over GATT |
| **Xbox** 主机 | **不行**（这套射频） | 只认 Xbox Wireless / 授权手柄 |
| **Switch 1/2、PS4/PS5** | **nRF52840 不行** | 主机要 **经典蓝牙** + 官方手柄那一套，不是 BLE |

主机蓝牙若一定要做，**不要自己在 nRF52 上仿，也不要把 [LiteEMF](https://github.com/LiteEMF/LiteEMF) 接到这颗 BLE 芯片上当主机方案。** LiteEMF 只是手柄中间件：HID 描述符、按键打包、`TR_EDR` 经典蓝牙。PS 那路还要外挂加密芯片（`ps_crypt` / NXP 7105），不是“一份开源逻辑就能连 PS5”。nRF52840 **没有 EDR**，跑不了那套。

国内现成的是山弯 **BM769P** 交钥匙 IC（客户给的烧录 exe）。Xbox 主机无线这颗同样做不了。

| | nRF52840（已锁定，电脑/手机） | 山弯 BM769P（可选，主机蓝牙） |
|---|---|---|
| 射频 | 只有 BLE + 私有 2.4G | 经典蓝牙，固件里带 NS/PS 模式 |
| 固件 | 我们可以写 BLE HID | **只能用山弯烧录器和他们的 bin**，仓库不实现主机仿冒 |
| 和 M487 | UART 从机，有线 8 kHz 仍走 M487 | 一般是整颗手柄 SoC；USB 多半是 FS，**不是 8 kHz** |
| 和 CH32V305 U 盘 | nRF52840 可以按 nRF24 发给 Si24R1 | **对不上** 我们的 2.4G 接收器 |

所以：电脑 8 kHz 有线、电脑 4 kHz U 盘、手机蓝牙 → 仍是 M487 + nRF52840 + CH32V305。**主机蓝牙是另一颗山弯 BM769P**，向山弯买料、用他们的升级程序，不要和 nRF 那条无线混焊。Xbox 无线这颗也做不了。

---

## 明确不采用

| 候选 | 原因 |
|---|---|
| nRF52820 / nRF52840 当接收器 | USB FS，只有 1 kHz；52820 还贵 |
| CH552 + Si24R1 | 便宜，但是 USB FS，只有 1 kHz |
| ESP32-S3 SuperMini | USB FS；Wi‑Fi 模组，不是 U 盘 |
| nRF24L01+PA+LNA 绿板 | 太大，不做进接收器 |
| 手柄无线改 ESP32-C3 | BLE 兼容性不如 nRF52840 |

备选（同样 USB HS、也有开发板，略贵）：雅特力 **AT32F405** + **AT-START-F405**。默认仍走沁恒 305/307。

---

## 速率

| 模式 | 上限 | 原因 |
|---|---|---|
| 有线（M487 CON1） | **8 kHz** | USB HS，`bInterval = 1` |
| 蓝牙 | 约 **133 Hz** | BLE 连接间隔 |
| **2.4G U 盘** | **4 kHz** | USB HS，`bInterval = 2`（250 µs）。主机若只给 1 kHz 是系统调度问题，硬件按 4 kHz 报 |

---

## 评估板接线

### 手柄：M487 ↔ nRF52840

| NuMaker-PFM-M487 | XIAO nRF52840 | 作用 |
|---|---|---|
| D1 **PB.3** UART1_TXD | **D7 / RX** | 1 Mbps `WfFrame`（1 Mbps 够喂 4 kHz） |
| D0 PB.2 UART1_RXD | TX（可空） | 调试 |
| NU7.7 **PA.11** | **D2** | CON1 插上后无线静音 |
| 3V3 / GND | 3V3 / GND | |

有线只用 **CON1**。

### 接收器：CH32V307V-EVT ↔ Si24R1

EVT 的 **USB 高速口（P6）** 插电脑。Si24R1：CE、CSN、SCK、MOSI、MISO、IRQ、3V3、GND。具体 GPIO 写程序时再钉到 EVT 空闲脚。

同一台电脑：有线、蓝牙、2.4G 不要同时连。

---

## 仓库（程序先停）

| 路径 | 处理 |
|---|---|
| `firmware/nuvoton_m487/` | 主控，保留 |
| `wireless/nrf52840/` | 手柄 BLE + ESB 发射草稿 |
| 接收器固件 | **尚未写。** 目标：CH32V305 USBHS HID 4 kHz + Si24R1 RX |
| `firmware/esp32c3_ble/` | 废弃 |
| `firmware/ch585/` | 废弃（那是当主控用沁恒，和接收器这颗 305 不是一回事） |
| `firmware/nuvoton_m032bt/` | 不采用 |

确认 BOM 后再写：M487 有线；手柄 nRF52840；**CH32V305 + Si24R1** 接收器。
