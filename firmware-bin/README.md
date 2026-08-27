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

开机按住按键再上电 / 插 USB（和 GP2040-CE 一样，A=B1）：

| 按键 | 模式 |
|------|------|
| A（B1） | Switch Pro（有线 USB + 蓝牙 + 2.4G） |
| B（B2） | Xbox 360（XInput） |
| X（B3） | PS3 |
| Y（B4） | PS4（固件已内置认证密钥，不用网页上传） |
| R1 | Xbox One（有线，USB 引导认证） |
| L1 | PS5（有线，USB 引导认证） |
| L2 | P5 General |
| R2 | 键盘 |

Xbox One / PS5 把官方或兼容认证狗插在板上 **USB-C AUTH** 口。PS4 用编进固件的 `serial.txt` / `sig.bin` / `key.pem`，不必再去网页后台传。
网页配置：手柄插电脑后打开 https://192.168.7.1

**插着手柄 USB 时无线不发射。** 用蓝牙 / 2.4G 时拔掉手柄 USB。

## 2.4G 接收器

1. 按住 BOOT，插接收器 USB-C
2. 拖入 `GP2040-WF_receiver.uf2`
3. 插 Switch 底座或 PC（Steam 认 Pro Controller）
4. 手柄选 **L2 Switch Pro**，拔掉手柄 USB 后才发 2.4G

不要把接收器插 Xbox / PS 主机。

## 杰里 AC632N（Switch 蓝牙）——必须另烧

AC632N 是独立蓝牙 MCU。**不烧它，Switch 蓝牙不能用。**  
`GP2040-WF_Pico16.uf2` 只进 RP2040。

现成烧录文件（GitHub Actions / 本地用官方 Linux 工具链编出来的）：

| 文件 | 给谁用 |
|------|--------|
| `GP2040-WF_AC632N.ufw` | 杰理烧录助手 / 强制升级 |
| `GP2040-WF_AC632N_app.bin` | 下载器里选 APP |
| `GP2040-WF_AC632N_jl_isd.bin` | 整片镜像（offset 0） |

模块 UART **RX** 接 Pico **GP0**（1 Mbps）。杰里这边默认 RX = **PA5**（`sdk_glue.c` 里 `WF_JL_UART_RX_PIN`）。蓝牙名 `Pro Controller`。

自己编：`.github/workflows/jieli-ac632n.yml`，或 `firmware/jieli_ac632n/patch_sdk.sh` + `make ac632n_hid`。

只用有线 / 2.4G 可以不焊这颗芯片。

## 手柄 Si24R1 脚位（Pico16）

CE GP1 · CSN GP21 · SCK GP25 · MOSI GP29 · MISO 可不接  
接收器多接 MISO **GP16**
