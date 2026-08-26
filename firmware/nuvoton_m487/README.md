# 新唐 M487：有线 USB HS 8 kHz HID 手柄

这是 **GP2040-WF** 现在的主固件。芯片是 **新唐 Nuvoton M487**（M480 系列，USB 2.0 High Speed），不是南京沁恒 WCH，也不是 RP2040。

官方 8K HID 鼠标例程就是这个 USB HS 外设：把 `HID_DEFAULT_INT_IN_INTERVAL` 从 BSP 默认的 `10` 改成 `1`。USB 2.0 HS 的 `bInterval=1` 是 \(2^{0}\times 125\,\mu s = 125\,\mu s\)，也就是 **8000 Hz**。

## 一块芯片做不到三模 8 kHz

新唐没有「USB HS 8k + 片内 BLE + 片内 802.11」的单芯片。分工如下：

| 模式 | 芯片 | 板子 | 轮询 |
|---|---|---|---|
| **有线 8 kHz** | **M487 / M484** USB HS | [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/) CON1 | 8 kHz |
| 蓝牙 + 私有 2.4G | M032BT | NuMaker-M032BT | USB 只有 FS，有线最高 1 kHz |
| 家用 Wi-Fi（802.11） | 不是 M487 片内 | NuMaker-IoT-M487 板上的 Wi-Fi 模组 | 不能当 8 kHz HID |

手柄圈说的「wifi / 2.4G」一般是 2.4G 接收器，走 M032BT 的私有射频，不是连路由器。说明见 [../nuvoton_m032bt/README.md](../nuvoton_m032bt/README.md)。

## 买什么、插哪个口

买 **NuMaker-PFM-M487**（M487JIDAE）。

1. **ICE** 口（Nu-Link-Me）：Keil / NuMicro ICP 烧录 `build/controller.hex`
2. **CON1**（板上标 USB HS / OTG）：打游戏。设备名 `GP2040-WF`，VID `0416` PID `B204`
3. 不要拿 CON2（USB FS）打 8 kHz，那个口最多 1 kHz

按键接到 Arduino 排针，脚位见 `include/wf_pins.h`。板上 SW2（PF.11）当 Start。RGB 红灯 PH.0，有键按下就亮。

## 编译

```bash
sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
cd firmware/nuvoton_m487
make test
make
```

得到 `build/controller.hex` 和 `build/controller.bin`。

Windows 用 [NuMicro ICP](https://www.nuvoton.com/tool-and-software/icp-programming-tool/) 或 Keil + Nu-Link。Linux 可用 OpenOCD / pyocd 对板上 CMSIS-DAP。

`bsp/` 是 [M480BSP](https://github.com/OpenNuvoton/M480BSP) 的精简子集（Apache-2.0）。USB 中断路径改自官方 `HSUSBD_HID_Mouse` 例程。
