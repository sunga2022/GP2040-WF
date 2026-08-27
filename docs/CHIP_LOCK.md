# 三模：GP2040-CE 原样 + Switch 无线 + 有线认证

主控还是 **RP2040**，固件还是仓库根目录这套 **GP2040-CE**。不换 MCU，不重写手柄协议。

| 模式 | 做法 |
|------|------|
| 有线 Xbox / PS | 手柄 USB。认证类型仍是 CE 的 `INPUT_MODE_AUTH_TYPE_USB`（引导认证） |
| 有线 Switch | 手柄 USB。CE 的 Switch / Switch Pro |
| 蓝牙 Switch | `WirelessAddon` UART → **杰里 AC632N**。Switch Pro 报告 0x30 + 0x01 子命令，**没有加密** |
| 2.4G Switch | 同一插件发 Si24R1 → **Pico 接收器** USB Switch Pro（与 CE `SwitchProDriver` 同一套描述符） |

USB 已经枚举时不发无线。

## 手柄接线

### Pico16（完整三模）

| 功能 | Pico16 GPIO |
|------|-------------|
| 杰里 UART RX | **GP0** UART0 TX，1 Mbps 8N1 |
| Si24R1 CE | **GP1** |
| Si24R1 CSN | **GP21** |
| Si24R1 SCK | **GP25** |
| Si24R1 MOSI | **GP15** |
| 电量 ADC | **GP29**（分压，ADC3） |
| L2 | **GP5**（原 FN；FN 已取消） |
| Si24R1 MISO | 手柄发射端可不接 |

开机 **A（B1）** = Switch Pro（`BoardConfig.h` 里 `DEFAULT_INPUT_MODE_B1`）。这个模式下无线才走 Switch 协议。B = Xbox 360，X = PS3，Y = PS4（内置密钥），R1 = Xbox One、L1 = PS5 只走手柄 USB 认证。

### G2（有线 + 蓝牙）

| 功能 | G2 GPIO |
|------|---------|
| 杰里 UART RX | **GP24** UART1 TX，1 Mbps 8N1 |

### 杰里 AC632N（Switch 蓝牙）

- 蓝牙名：**Pro Controller**
- VID `0x057E` PID `0x2009`，打开 EDR HID
- HID 描述符用 `wireless/switch_pro_desc.h`
- UART 中断：`wf_jieli_on_byte`；HID 下行：`wf_jieli_on_hid_output`；主循环：`wf_jieli_poll`
- 源码：[`firmware/jieli_ac632n/`](../firmware/jieli_ac632n/)
- Switch Pro 状态机：[`wireless/switch_pro.c`](../wireless/switch_pro.c)（从 CE `SwitchProDriver` 抽出来的 C，无加密）

## 2.4G 接收器

接收器 USB 就是 CE 那套 Switch Pro：VID `057E` PID `2009`，64 字节报告，处理 0x80 握手和 0x01 子命令。插 Switch 底座 USB 或 PC（Steam 认 Pro）。

| nRF24 / Si24R1 | 接收器 Pico |
|----------------|-------------|
| CE | GP1 |
| CSN | GP21 |
| SCK | GP25 |
| MOSI | GP29 |
| MISO | **GP16** |
| VCC / GND | 3V3 / GND |

空中包仍是 20 字节 `WfFrame`。Xbox / PS 不要插这块接收器，插手柄 USB。
