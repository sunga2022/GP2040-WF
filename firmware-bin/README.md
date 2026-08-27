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

OLED 右上角显示 **87%B** 这种百分比 + **L** 插线 / **B** 蓝牙 / **G** 2.4G。

双色电量灯：按键灯串后面再串一颗 WS2812（Pico16 下标 14，还是 GP28）。正常绿，低于约 3.5 V 红，插 USB 一定绿。电量 ADC 在 **GP29**（100k/100k 分压，`BATTERY_ADC_SCALE` 2.0）。不要把电芯正极直接焊到 GP29。

快捷键（A2=TOUCH，S2=START，S1=SELECT）写进固件，开机强制覆盖网页配置：

- A2+START+LEFT 左摇杆 · A2+START+RIGHT 右摇杆 · A2+START+UP 十字键
- A2+SELECT+UP 上优先 · A2+SELECT+LEFT 后输入优先 · A2+SELECT+DOWN 回中 · A2+SELECT+RIGHT 先输入优先

RGB 只跟灯珠串联顺序，改键值不会把灯挪到别的位置。USB-C AUTH 认证口默认打开。PS5 固定为带 NXP7105 的街机手柄类型 7（USB 引导认证），不要改成 DualShock 仿冒。

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

CE GP1 · CSN GP21 · SCK GP25 · MOSI **GP15** · MISO 可不接  
电量 ADC **GP29**（分压）· L2 **GP5** · 无 FN  
接收器多接 MISO **GP16**（接收器 MOSI 仍是 GP29）
