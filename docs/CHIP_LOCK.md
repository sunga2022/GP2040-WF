# GP2040-WF 芯片与方案锁定

**状态：已锁定。先按本文买板、对原理图，再写固件。**

仓库里现有的 ESP32-C3 / ESP32-S3 / CH585 代码是选型过程留下的，**新板不要按那些 BOM 做。**

三模只这三件事，没有家用 Wi‑Fi、没有 802.11、不连路由器：

1. 有线 USB
2. 蓝牙 HID
3. 2.4G USB 接收器（和鼠标接收器一类的私有 2.4G，不是 Wi‑Fi）

---

## 锁定的三颗料

| 角色 | 芯片（必须写进 BOM 的料号） | 现在就买的板 | 干什么 |
|---|---|---|---|
| 手柄主控 | **新唐 Nuvoton M487JIDAE** | [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/) | 读按键；**CON1 USB High Speed 8 kHz**；UART 把按键帧送给无线模组 |
| 手柄无线 | **Nordic nRF52840-QIAA** | [Seeed XIAO nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/) | **BLE HID** + 私有 **2.4G 发射**（Enhanced ShockBurst） |
| **2.4G 接收器** | **Nordic nRF52840-QIAA**（与手柄无线同一颗） | 再买一块 **Seeed XIAO nRF52840**，只插电脑 USB | 私有 **2.4G 接收** + **USB HID 手柄** |

量产可以把两块 XIAO 换成任意 **nRF52840-QIAA 模组**（安信可、嘉立创、自画小板），**芯片不能换家族**。接收器若要成品 U 盘外形，可用 Nordic **nRF52840 Dongle（PCA10059）**，硅片仍是 nRF52840。

```
按键 ──► M487 ── CON1 USB HS ──► 电脑          仅此路径 8 kHz
            │
            └── UART1 ──► nRF52840（手柄）── BLE ──► 手机/电脑
                               └── ESB 2.4G ──► nRF52840 接收器 USB ──► 电脑
```

CON1 枚举成功后，M487 把 **PA.11** 拉高，手柄上的 nRF52840 **关掉 BLE 广播和 2.4G 发射**，避免和有线抢同一台电脑。

---

## 为什么 2.4G 接收器锁定 nRF52840

接收器和手柄无线必须是**同一套空中协议**。手柄侧已经要用一颗能打 BLE 的 2.4G SoC，接收器就用同一颗，只跑 RX + USB，不再加第三家射频。

| 候选 | 结论 |
|---|---|
| **nRF52840**（锁定） | 片内 BLE 5 + 私有 2.4G（ESB）+ USB Device。手柄、接收器同一颗料。空中协议不是 802.11。XIAO 国内好买。 |
| ESP32-S3 / C3 | Wi‑Fi SoC。ESP-NOW 走的是 Wi‑Fi PHY，和「不要家用 Wi‑Fi、只要 2.4G 接收器」冲突。C3 在 Arduino 2.x 上做不了 USB HID，不能当接收器。 |
| 新唐 M032BT | 新唐自己的 BLE+2.4G，但 USB 只有 Full Speed；射频要 Nuvoton BLE/RF BSP，上手比 nRF52840 重。主控已经是 M487，无线不必再绑一颗难写的新唐射频。 |
| nRF24L01+ + 额外 USB 单片机 | 接收器要两颗料；手柄还要另买 BLE。比一颗 nRF52840 麻烦。 |
| 沁恒 CH585 | 认错厂商。用户要的是**新唐 Nuvoton**，不是沁恒 WCH。 |

**接收器芯片写进 BOM 的名字就是：`nRF52840-QIAA`。** 不要再写「再选一颗 ESP32」。

---

## 速率（不要对外乱讲 8 kHz）

| 模式 | 上限 | 原因 |
|---|---|---|
| 有线（M487 CON1） | **8 kHz** | USB 2.0 High Speed，`bInterval = 1` |
| 蓝牙 | 约 **133 Hz** | BLE HID 连接间隔下限约 7.5 ms |
| 2.4G 接收器 | 约 **1 kHz** | nRF52840 的 USB 是 **Full Speed**，主机轮询最快 1 ms |

空中 2.4G 可以打得很快；电脑看到的轮询被 USB FS 卡住。**8 kHz 只宣传有线。**

---

## 评估板接线（锁定）

### 手柄：M487 ↔ nRF52840

| NuMaker-PFM-M487 | XIAO nRF52840 | 作用 |
|---|---|---|
| D1 **PB.3** UART1_TXD | **D7 / RX** | 1 Mbps 按键帧 `WfFrame` |
| D0 PB.2 UART1_RXD | TX（可空） | 调试回读，量产可不接 |
| NU7.7 **PA.11** | **D2** | CON1 插上后为高，无线静音 |
| 3V3 / GND | 3V3 / GND | 共地共 3.3 V |

有线 USB 只用板上 **CON1**（HS），不要用 CON2 那路 FS。

### 2.4G 接收器

XIAO nRF52840 **只插电脑 USB**。不接 M487，不接手柄上的那块 XIAO 的线。两块 nRF 靠私有 2.4G 对上。

同一台电脑：**有线、蓝牙、2.4G 不要同时连**。拔 CON1 再用无线。

---

## 现成仓库怎么看待（程序先停）

| 路径 | 处理 |
|---|---|
| `firmware/nuvoton_m487/` | **保留。** 主控方案已锁定，有线 8 kHz 可以在确认 BOM 后继续改。 |
| `wireless/nrf52840/` | **这才是无线方向。** 现有草稿可作起点，锁定前不再加功能。 |
| `firmware/esp32c3_ble/` | **废弃。** C3 手柄 + S3 接收器不是本方案。 |
| `firmware/ch585/` | **废弃。** 沁恒，认错厂商。 |
| `firmware/nuvoton_m032bt/` | **不采用。** 仅作备忘。 |
| 根目录 GP2040-CE / Pico | **旧板。** 新板不要 RP2040。 |

确认本文之后，固件只做三件事：M487 有线 + UART；手柄 nRF52840 的 BLE 与 2.4G 发射；接收器 nRF52840 的 2.4G 接收与 USB HID。
