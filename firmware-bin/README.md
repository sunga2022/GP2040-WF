# 烧录文件（拖进 RPI-RP2）

两块 RP2040 都是 **按住 BOOT → 插 USB → 出现 `RPI-RP2` 磁盘 → 拖入对应 UF2**。

| 文件 | 烧到哪 |
|------|--------|
| `GP2040-WF_Pico16.uf2` | 手柄主板（Pico16 三模：有线 + 杰里蓝牙 + Si24R1 2.4G） |
| `GP2040-WF_receiver.uf2` | 2.4G 接收器（USB 枚举成 Switch Pro `057E:2009`） |

## 手柄

1. 按住板上 **BOOT**
2. 用 **USB-C 设备口**（不是认证口）插电脑
3. 把 `GP2040-WF_Pico16.uf2` 拖进 `RPI-RP2`
4. 绿灯闪完自动弹出即可

开机按住按键再上电 / 插 USB：

| 按键 | 模式 |
|------|------|
| R1 | Xbox（有线，USB 引导认证） |
| L1 | PS5（有线，USB 引导认证） |
| B4 | PS4（有线，USB 引导认证） |
| L2 | Switch Pro（有线 USB + 蓝牙 + 2.4G） |

Xbox / PS 把官方或兼容认证狗插在板上 **USB-C AUTH** 口。  
网页配置：手柄插电脑后打开 https://192.168.7.1

**插着手柄 USB 时无线不发射。** 用蓝牙 / 2.4G 时拔掉手柄 USB。

## 2.4G 接收器

1. 按住 BOOT，插接收器 USB-C
2. 拖入 `GP2040-WF_receiver.uf2`
3. 插 Switch 底座或 PC（Steam 认 Pro Controller）
4. 手柄选 **L2 Switch Pro**，拔掉手柄 USB 后才发 2.4G

不要把接收器插 Xbox / PS 主机。

## 杰里 AC632N（Switch 蓝牙）

没有 UF2。用杰里官方 `fw-AC63_BT_SDK` HID 工程：

- 加入仓库里 `firmware/jieli_ac632n/` 和 `wireless/switch_pro.c`
- 蓝牙名 `Pro Controller`，VID `057E` PID `2009`
- Pico **GP0** → 杰里 UART RX，1 Mbps 8N1

## 手柄 Si24R1 脚位（Pico16）

CE GP1 · CSN GP21 · SCK GP25 · MOSI GP29 · MISO 可不接  
接收器多接 MISO **GP16**
