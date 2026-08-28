# 烧录（就这三个文件）

| 文件 | 烧到哪 |
|------|--------|
| `GP2040-WF.uf2` | 手柄 RP2040。按住 BOOT，USB-C **设备口** 出现 `RPI-RP2` 后拖进去 |
| `GP2040-WF_AC632N.ufw` | 杰理 **AC632N / AC6321A**。杰理烧录助手选 AC632N。不要烧到 AC6956A |
| `GP2040-WF_receiver.uf2` | 2.4G USB-A 接收器 RP2040。按住 BOOT 再插电脑 |

`SHA256SUMS` 是校验，不能烧。杰理的 `app.bin` / `jl_isd.bin` 是同一份固件的别的格式，日常用 `.ufw` 即可。

## 手柄 Pico19

FN 已取消。OLED GP0/GP1，右上角 `87%B`（电量% + **L** 插线 / **B** 蓝牙 / **G** 2.4G）。RGB GP28，灯序只跟线。认证口 D+ GP3、D− GP2。

开机按住（A=B1）：**A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘**

PS4 用固件里的三份授权文件。PS5 / Xbox One 走认证口 NXP7105。Xbox 360 不用额外认证。插着手柄 USB 时无线不发。

快捷键 TOUCH=A2 START=S2 SELECT=S1：

- A2+START+LEFT 左摇杆 · A2+START+RIGHT 右摇杆 · A2+START+UP 十字键
- A2+SELECT+UP 上优先 · A2+SELECT+LEFT 后输入优先 · A2+SELECT+DOWN 回中 · A2+SELECT+RIGHT 先输入优先

网页配置：https://192.168.7.1

## 2.4G 接收器

键盘接收器那种 **USB-A 40×18 mm**，芯片用 RP2040（和手柄同一套 UF2 工具，Switch Pro `057E:2009` 已经编好）。CH552 更小，但要另写 USB 栈、不能拖 UF2。

插 Switch 底座或 PC（Steam 认 Pro）。手柄选 Switch Pro，拔掉手柄 USB。不要插 Xbox / PS。
