# 烧录

两块 RP2040 都是 **按住 BOOT，插 USB，拖 UF2**。

## 手柄主板（Pico16 三模）

文件：`firmware-bin/GP2040-WF_Pico16.uf2`

1. 按住板上 BOOT
2. 插 USB-C 设备口到电脑
3. 出现 `RPI-RP2` 磁盘
4. 把 `GP2040-WF_Pico16.uf2` 拖进去，绿灯闪完即可

开机按键（按住再插 USB / 上电，和 GP2040-CE 一样，A=B1）：

| 按键 | 模式 |
|------|------|
| A（B1） | Switch Pro（有线 USB + 蓝牙 + 2.4G） |
| B（B2） | Xbox 360（XInput） |
| X（B3） | PS3 |
| Y（B4） | PS4（固件已内置认证密钥） |
| R1 | Xbox One（USB 引导认证） |
| L1 | PS5（USB 引导认证） |
| L2 | P5 General |
| R2 | 键盘 |

Xbox One / PS5 把官方或兼容认证狗插在板上 **USB-C AUTH** 口。PS4 不用网页上传密钥。

网页配置：https://192.168.7.1 （手柄插电脑时）

OLED 右上角：**87%B** 这种百分比 + **L** 插线 / **B** 蓝牙 / **G** 2.4G。电量 ADC 在 **GP29**（分压，默认 100k/100k）。WS2812 灯串多一颗，正常绿、低于约 3.5 V 红。

快捷键锁死：A2+START+左/右/上 = 左摇杆 / 右摇杆 / 十字键；A2+SELECT+上/左/下/右 = 上优先 / 后输入优先 / 回中 / 先输入优先。

RGB 按线路顺序，不跟键值。认证口默认打开。PS5 按 NXP7105 街机手柄（类型 7）做 USB 引导认证。

## 2.4G 接收器

文件：`firmware-bin/GP2040-WF_receiver.uf2`

1. 按住 BOOT，插接收器 USB-C
2. 拖入 UF2
3. 插 Switch 底座或 PC（Steam 认 Pro Controller）
4. 手柄拔掉 USB 后才发 2.4G

## 杰里 AC632N（Switch 蓝牙）——必须另烧

**要装固件。** UF2 烧不进去。模块出厂程序不会冒充 Switch Pro，也听不懂 GP0 上的 UART。

现成文件：`firmware-bin/GP2040-WF_AC632N.ufw`（杰理烧录助手）和 `GP2040-WF_AC632N_app.bin`。  
GitHub Actions 工作流 `Jieli AC632N HID` 用官方 Linux 工具链 + `make ac632n_hid` 编出来。

Pico **GP0** → 杰里 UART RX（默认芯片脚 PA5），1 Mbps。

只用有线或 2.4G 接收器时，可以不焊、不烧 AC632N。

## 接线速查

手柄 Si24R1：CE GP1，CSN GP21，SCK GP25，MOSI **GP15**  
电量 ADC：**GP29**（分压，不要直接接 4.2 V）  
L2：**GP5**（原 FN 脚）。没有 FN 键。  
接收器多 MISO GP16  
杰里：GP0 TX
